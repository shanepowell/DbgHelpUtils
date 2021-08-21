// AllocationSetupTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <boost/program_options.hpp>

int LfhAllocations(std::wstring const& alloc_dump_filename, std::wstring const& free_dump_filename);
int VirtualAllocations(std::wstring const& alloc_dump_filename, std::wstring const& free_dump_filename);
void GenerateDumpFile(std::wstring const& dump_filename);


int main(int const argc, char* argv[])
{
    namespace po = boost::program_options;
    try
    {
        try
        {
            po::options_description options;
            options.add_options()
                ("help,h", "produce help message")
                ("lfh", "generate lfh allocations")
                ("virtual", "generate virtual allocations")
                ("alloc", po::wvalue<std::wstring>(), "dump post allocate filename")
                ("free", po::wvalue<std::wstring>(), "dump post free filename")
                ;

            po::variables_map vm;
            store(parse_command_line(argc, argv, options), vm);
            notify(vm);

            auto const do_lfh_allocations = vm.count("lfh") != 0;
            auto const do_virtual_allocations = vm.count("virtual") != 0;
            auto const alloc_dump_filename = vm["alloc"].as<std::wstring>();
            auto const free_dump_filename = vm["free"].as<std::wstring>();

            if (vm.count("help") || (!do_lfh_allocations && !do_virtual_allocations))
            {
                std::cout << options << "\n";
                return EXIT_SUCCESS;
            }

            if(do_lfh_allocations)
            {
                return LfhAllocations(alloc_dump_filename, free_dump_filename);
            }

            if(do_virtual_allocations)
            {
                return VirtualAllocations(alloc_dump_filename, free_dump_filename);
            }

            return EXIT_SUCCESS;
        }
        catch (std::exception const& e)
        {
            std::cout << "fatal error: " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "fatal error: Unknown exception\n";
        }
    }
    catch(...)
    {
    }
    return EXIT_FAILURE;
}

int LfhAllocations(std::wstring const& alloc_dump_filename, std::wstring const& free_dump_filename)
{
    auto const heap = GetProcessHeap();

    std::array<void*, 0x12> backend_allocations{};
    constexpr auto allocation_size = 0x10;

    char fill_value = 'A';
    for (auto& backend_allocation : backend_allocations)
    {
        std::cout << "allocating backend [" << fill_value << "] buffer\n";
        backend_allocation = HeapAlloc(heap, 0x0, allocation_size);
        if(backend_allocation == nullptr)
        {
            std::cout << "Failed to allocate backend allocation\n";
            return EXIT_FAILURE;
        }
        memset(backend_allocation, fill_value, allocation_size);
        ++fill_value;
    }

    std::array<void*, 0x12> frontend_allocations{};
    for (auto& frontend_allocation : frontend_allocations)
    {
        std::cout << "allocating frontend [" << fill_value << "] buffer\n";
        frontend_allocation = HeapAlloc(heap, 0x0, allocation_size);
        if(frontend_allocation == nullptr)
        {
            std::cout << "Failed to allocate frontend allocation\n";
            return EXIT_FAILURE;
        }
        memset(frontend_allocation, fill_value, allocation_size);
        ++fill_value;
    }

    if(alloc_dump_filename.empty())
    {
        std::cout << "press enter to free\n";
        [[maybe_unused]] auto const ch1 = getchar();
    }
    else
    {
        GenerateDumpFile(alloc_dump_filename);
    }

    HeapFree(heap, 0x0, backend_allocations[0]);
    HeapFree(heap, 0x0, frontend_allocations[0]);

    if(free_dump_filename.empty())
    {
        std::cout << "press enter to exit\n";
        [[maybe_unused]] auto const ch2 = getchar();
    }
    else
    {
        GenerateDumpFile(free_dump_filename);
    }

    return EXIT_SUCCESS;
}


int VirtualAllocations(std::wstring const& alloc_dump_filename, std::wstring const& free_dump_filename)
{
    auto const heap = GetProcessHeap();

    std::array<void*, 0x5> virtual_allocations{};
    // ReSharper disable once CppTooWideScope
    constexpr auto allocation_size = 0x100000; // 1mb

    char fill_value = 'A';
    for (auto& virtual_allocation : virtual_allocations)
    {
        std::cout << "virtual allocation [" << fill_value << "] buffer\n";
        virtual_allocation = HeapAlloc(heap, 0x0, allocation_size);
        if(virtual_allocation == nullptr)
        {
            std::cout << "Failed to allocate virtual allocation\n";
            return EXIT_FAILURE;
        }
        memset(virtual_allocation, fill_value, allocation_size);
        ++fill_value;
    }


    if(alloc_dump_filename.empty())
    {
        std::cout << "press enter to free\n";
        [[maybe_unused]] auto const ch1 = getchar();
    }
    else
    {
        GenerateDumpFile(alloc_dump_filename);
    }

    HeapFree(heap, 0x0, virtual_allocations[0]);
    HeapFree(heap, 0x0, virtual_allocations[3]);

    if(free_dump_filename.empty())
    {
        std::cout << "press enter to exit\n";
        [[maybe_unused]] auto const ch2 = getchar();
    }
    else
    {
        GenerateDumpFile(free_dump_filename);
    }

    return EXIT_SUCCESS;
}

void GenerateDumpFile(std::wstring const& dump_filename)
{
    std::wostringstream ss;

    // ReSharper disable once StringLiteralTypo
    ss << "procdump -ma " << GetCurrentProcessId() << " \"" << dump_filename << "\"";
    auto const command = std::move(ss).str();


    STARTUPINFOW startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(startup_info));
    memset(&process_info, 0, sizeof(process_info));
    startup_info.cb = sizeof(startup_info);

    if(command.size() >= MAX_PATH)
    {
        std::wcout << "Command line [" << command << "] to long : " << command.size() << " >= " << MAX_PATH << '\n';
        return;
    }

    wchar_t temp[MAX_PATH];
    wcsncpy_s(temp, command.c_str(), command.size());
    temp[command.size()] = 0;

    std::wcout << "Run: [" << command << "]";
    if (!CreateProcessW(nullptr, temp, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info))
    {
        std::wcout << "CreateProcessW [" << command << "] failed " << GetLastError() << '\n';
        return;
    }

    //Close thread handle. 
    CloseHandle(process_info.hThread);

    // Wait for dump to complete...
    WaitForSingleObject(process_info.hProcess, INFINITE);

    // Close process handle.
    CloseHandle(process_info.hProcess);
}
