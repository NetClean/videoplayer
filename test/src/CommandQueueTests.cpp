#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <limits>
#include <string>
#include <thread>

#include <SDL.h>

#include "CommandQueueTests.h"
#include "CommandQueue.h"
#include "Flog.h"
#include "Pipe.h"

#define MAGIC 0xaabbaacc

class CCommandQueueTests : public CommandQueueTests
{
	public:
	void RegisterTests(std::vector<Test>& testSet)
	{
		testSet.push_back({"CommandQueue", "StartStop", [&]{StartStop();} });
		testSet.push_back({"CommandQueue", "DecodeCommands", [&]{DecodeCommands();} });
	}

	void StartStop()
	{
		PipePtr pipe = Pipe::Create();
		pipe->CreatePipe(L"cmd");

		CommandQueuePtr cq = CommandQueue::Create();

		PipePtr cPipe = Pipe::Create();
		cPipe->Open(L"cmd");
		cq->Start(cPipe);

		// stop
		pipe->WriteUInt32(MAGIC);
		pipe->WriteUInt32(CTQuit);
		pipe->WriteUInt32(0);
		pipe->WriteUInt32(0);
		pipe->WriteUInt32(MAGIC);
	}

	void DecodeCommands()
	{
		PipePtr pipe = Pipe::Create();
		pipe->CreatePipe(L"cmd2");

		CommandQueuePtr cq = CommandQueue::Create();

		PipePtr cPipe = Pipe::Create();
		cPipe->Open(L"cmd2");

		cq->Start(cPipe);

		bool wasException = false;
		std::string ex;
		
		std::thread t([&](){
			try {
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTPlay);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(MAGIC);
				
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTPause);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(MAGIC);
				
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTStop);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(MAGIC);
				
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTSeek);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteFloat(0.34f);
				pipe->WriteUInt32(MAGIC);

				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTLoad);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteInt32(0);
				pipe->WriteString(L"name");
				pipe->WriteUInt32(MAGIC);
				
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTUnload);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(MAGIC);
				
				pipe->WriteUInt32(MAGIC);
				pipe->WriteUInt32(CTQuit);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(0);
				pipe->WriteUInt32(MAGIC);
			}

			catch (std::runtime_error e)
			{
				wasException = true;
				ex = e.what();
			}
		});

		int nCmd = 1;
		int tries = 0;

		try {
			while(true){
				Command c;
				if(cq->Dequeue(c)){
					CommandType type = (CommandType)nCmd;

					if(c.type != CTQuit){
						TAssertEquals(c.type, type);
					}

					auto argTypes = CommandSpecs[(int)c.type].requestArgTypes;

					TAssertEquals(argTypes.size(), c.args.size());

					if(c.type == CTLoad){
						TAssertEquals(c.args[0].i, 0);
						TAssert(c.args[1].str == L"name", "string mismatch");
					}
					
					if(c.type == CTSeek){
						TAssertEquals(c.args[0].f, 0.34f);
					}

					if(c.type == CTQuit){
						break;
					}
					
					nCmd++;
				}
				else{
					SDL_Delay(1);
				}

				if(tries++ > 10000)
					throw std::runtime_error("too many tries");
			}
		}

		catch (std::runtime_error e)
		{
			t.join();
			throw;
		}

		t.join();

		if(wasException)
			throw std::runtime_error(ex);
	}
};

CommandQueueTestsPtr CommandQueueTests::Create()
{
	return std::make_shared<CCommandQueueTests>();
}
