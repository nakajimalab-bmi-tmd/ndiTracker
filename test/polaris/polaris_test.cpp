#include <cereal/cereal.hpp>
#include <conio.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <fstream>
#include <polaris.h>

std::vector<std::string> roms;

bool run = true;
bool ready = false;
size_t count = 0;

std::string getDatetimeStr() {
	time_t t = time(nullptr);
	const tm* localTime = localtime(&t);
	std::stringstream s;
	s << "20" << localTime->tm_year - 100;
	// setw(),setfill()‚Å0‹l‚ß
	s << std::setw(2) << std::setfill('0') << localTime->tm_mon + 1;
	s << std::setw(2) << std::setfill('0') << localTime->tm_mday;
	s << '_';
	s << std::setw(2) << std::setfill('0') << localTime->tm_hour;
	s << std::setw(2) << std::setfill('0') << localTime->tm_min;
	s << std::setw(2) << std::setfill('0') << localTime->tm_sec;
	// std::string‚É‚µ‚Ä’l‚ð•Ô‚·
	return s.str();
}

void record(std::string filename)
{
	std::ofstream ofs{ filename + '_' + getDatetimeStr() + ".csv" };
	ofs << "cpu time, time, qw, qx, qy, qz, x, y, z, err" << std::endl;

	using device::ndi::polaris;
	polaris p;

	p.auto_connect();
	for (auto const& rom : roms)
	{
		std::cout << "register passive tool: " << rom << std::endl;
		p.add_passive_tracker(rom.c_str());
	}
	auto ports = p.configure_active_trackers();

	std::cout << "number of tools: " << ports.size() << std::endl;
	std::cout << "Press any key for start recording. " << std::endl;

	while (!_kbhit());
	_getch();

	p.start(false);
	::Sleep(1000);
	ready = true;
	run = true;

	auto start_time = std::chrono::steady_clock::now();
	do
	{
		auto timer = std::chrono::steady_clock::now();
		p.tracking_one();
		p.update();

		ofs << std::chrono::duration_cast<std::chrono::milliseconds>(timer.time_since_epoch()).count()
			<< ", " << std::chrono::duration_cast<std::chrono::milliseconds>(timer - start_time).count();
		for (auto& port : ports)
		{
			auto const& tool = p.get_tracker_tool(port);
			auto data = tool.get_handle_data();
			if (data.status.is_valid())
			{
				auto const& transform_data = data.transform_data;
				for (auto const d : transform_data.data)
				{
					ofs << ", " << d;
				}
			}
			else
			{
				ofs << ",,,,,,,,";
			}
		}
		ofs << std::endl;
		++count;
		//while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timer).count() < 1.);
	} while (run);
	p.stop();
}

int main()
{
	{
		std::cout << "reading romfiles.txt..." << std::endl;
		std::ifstream ifs{ "romfiles.txt" };
		std::string buf;
		while (std::getline(ifs, buf))
		{
			std::ifstream ifs_{ buf };
			if (!ifs_) continue;
			std::cout << buf << std::endl;
			roms.push_back(buf);
		}
	}

	std::thread th{ record, "record" };

	while (!ready)
	{
		Sleep(100);
	}

	do
	{
		std::cout << "number of recorded frames: " << count << std::endl;
		::Sleep(100);
	} while (_kbhit() == 0);
	_getch();
	run = false;
	th.join();
	return 0;
}
