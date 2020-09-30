#include <conio.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>
#include <polaris.h>

using device::ndi::polaris;
polaris p;
bool run = true;
size_t count = 0;

void record(std::string filename)
{
	std::ofstream ofs{ filename };
	ofs << "time, qw, qx, qy, qz, x, y, z, err" << std::endl;
	do
	{
		p.tracking();
		ofs << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		auto tools = p.update_tracker_tools();

		for (auto& tool : tools)
		{
			auto data = tool.second.get_handle_data();
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
	} while (run);
}

int main()
{
	p.auto_connect();
	auto ports = p.configure_active_trackers();
	p.start(false);
	std::thread th{ record, "record.csv" };
	do
	{
		std::cout << "number of recorded frames: " << count << std::endl;
		::Sleep(100);
	} while (_kbhit() == 0);
	_getch();
	run = false;
	p.stop();

	return 0;
}
