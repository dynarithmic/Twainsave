#define PIPE_NAME L"\\\\.\\pipe\\TwainSaveBroker_v1"
#define PIPE_BUFSIZE 4096
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <string>
#include "twainsave_broker.h"
#include "commandline.h"
#include "twainsave_app.h"
#include "dynarithmic\twain\utilities\string_utilities.hpp"

twainsave_broker::twainsave_broker() : m_twainSession(dynarithmic::twain::startup_mode::none)
{}

HANDLE twainsave_broker::CreateBrokerPipe()
{
	HANDLE hPipe = CreateNamedPipeW(
		PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		PIPE_BUFSIZE,
		PIPE_BUFSIZE,
		0,
		nullptr
	);

	if (hPipe == INVALID_HANDLE_VALUE)
		return nullptr;

	BOOL connected = ConnectNamedPipe(hPipe, nullptr) ?
		TRUE :
		(GetLastError() == ERROR_PIPE_CONNECTED);

	if (!connected)
	{
		CloseHandle(hPipe);
		return nullptr;
	}

	return hPipe;
}

bool twainsave_broker::ReadLine(std::string& line)
{
	line.clear();
	char ch;
	DWORD bytesRead;

	while (true)
	{
		if (!ReadFile(m_pipe, &ch, 1, &bytesRead, nullptr) || bytesRead == 0)
			return false; // disconnected

		if (ch == '\n')
			break;

		if (ch != '\r')
			line.push_back(ch);
	}
	return true;
}

bool twainsave_broker::ReadFullMessage(std::string& out)
{
	out.clear();
	char buffer[PIPE_BUFSIZE];
	DWORD bytesRead = 0;

	while (true)
	{
		BOOL ok = ReadFile(
			m_pipe,
			buffer,
			sizeof(buffer),
			&bytesRead,
			NULL
		);

		if (!ok)
		{
			DWORD err = GetLastError();

			if (err == ERROR_MORE_DATA)
			{
				out.append(buffer, bytesRead);
				continue; // keep reading same message
			}

			return false; // pipe closed or error
		}

		out.append(buffer, bytesRead);
		break; // message complete
	}

	return true;
}

bool twainsave_broker::ReadPipeCommand(std::string& outCmd)
{
	char buffer[PIPE_BUFSIZE];
	DWORD bytesRead = 0;

	BOOL ok = ReadFile(m_pipe,buffer,sizeof(buffer) - 1,&bytesRead,nullptr);

	if (!ok || bytesRead == 0)
		return false;

	buffer[bytesRead] = 0;
	outCmd.assign(buffer);
	return true;
}

void twainsave_broker::WritePipeResponse(const std::string& msg)
{
	DWORD bytesWritten;
	WriteFile(m_pipe,msg.c_str(),(DWORD)msg.size(),&bytesWritten,nullptr);
}

HANDLE twainsave_broker::UseBroker()
{
	auto pipe = CreateBrokerPipe();
	m_pipe = pipe;
	return pipe;
}


int twainsave_broker::StartBroker(twainsave_app& theApp)
{
	m_twainSession.start();
	for (;;)
	{
		m_pipe = CreateBrokerPipe();
		if (m_pipe == NULL)
		{
			return RETURN_BROKER_ERROR;
		}

		std::string command;

		if (!ReadLine(command))
		{
			// Client disconnected or pipe broken
			break;
		}

		// Normalize command (trim CR/LF)
		dynarithmic::twain::trim_if(command, [](unsigned char ch) { return iscntrl(ch); });

		if (command == "SHUTDOWN")
		{
			WritePipeResponse("OK SHUTDOWN\n");
			break;
		}

		if (command == "RESET")
		{
			m_twainSession.stop();
		}
		else
		if (command.rfind("RUN ", 0) == 0)
		{
			// Get the command line
			auto new_string = command.substr(4);
			std::istringstream strm(new_string);
			CommandLine cmdLine(strm);
			auto args = cmdLine.get_arguments();
			auto parseRet = theApp.parse_options(args.size(), args.data());
			auto& allOptions = theApp.get_scanner_options();
			bool scanOk = false;
			if (parseRet.first)
			{
				auto ret = theApp.start_acquisitions(&m_twainSession);
				if (ret == RETURN_OK)
				{
					scanOk = true;
					WritePipeResponse("OK\n");
				}
			}
			if ( !scanOk )
			{
				auto retCode = allOptions.get_return_code();
				std::string retCodeStr = std::to_string(retCode);
				retCodeStr += "Error: " + retCodeStr +
					" (" + allOptions.m_ReturnCodesMap[retCode] + ")\n";
				WritePipeResponse(retCodeStr.c_str());
			}
		}

		else
			// Process normal command
			WritePipeResponse("OK\n");

		FlushFileBuffers(m_pipe);
		DisconnectNamedPipe(m_pipe);
		CloseHandle(m_pipe);
	}

	m_twainSession.stop();
	return RETURN_OK;
}