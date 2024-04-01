#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "core.h"

int main()
{
    using namespace std;

    //cout << "hello my name is" << "\b \b" << '\n';
	const auto file_contents = shared_ptr<uint8_t[]>(new uint8_t[0x100000]);

    ifstream file(R"(C:\Users\rshar\Desktop\Projects\School\SeniorProject\Logisim\test_prg.bin)", std::ios::binary);
    file.seekg(0, std::ios::end);
    const streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    memset(file_contents.get(), 0, 0x100000);
    file.read(reinterpret_cast<char*>(file_contents.get()), min(size, static_cast<streamsize>(0x100000)));
    file.close();

	auto core = RV32IM::Core();
	core.load_memory_contents(file_contents);

    int counter = 0;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (size_t idx = 0; idx < 1000000; idx++)
    {
        core.clock();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    //while (true)
    //{
    //    cout << format("\r{:#010x}", core.get_PC());
    //    //counter = core.get_PC();
    //    core.clock();
    //}
	return EXIT_SUCCESS;
}
