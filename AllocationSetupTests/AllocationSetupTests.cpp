// AllocationSetupTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <iostream>
#include <Windows.h>
#include <boost/program_options.hpp>

int LfhAllocations();
int VirtualAllocations();

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
                ;

            po::variables_map vm;
            store(parse_command_line(argc, argv, options), vm);
            notify(vm);

            auto const do_lfh_allocations = vm.count("lfh") != 0;
            auto const do_virtual_allocations = vm.count("virtual") != 0;

            if (vm.count("help") || (!do_lfh_allocations && !do_virtual_allocations))
            {
                std::cout << options << "\n";
                return EXIT_SUCCESS;
            }

            if(do_lfh_allocations)
            {
                return LfhAllocations();
            }

            if(do_virtual_allocations)
            {
                return VirtualAllocations();
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

int LfhAllocations()
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

    std::cout << "press enter to free\n";
    [[maybe_unused]] auto const ch1 = getchar();

    HeapFree(heap, 0x0, backend_allocations[0]);
    HeapFree(heap, 0x0, frontend_allocations[0]);
    std::cout << "press enter to exit\n";
    [[maybe_unused]] auto const ch2 = getchar();

    return EXIT_SUCCESS;
}


int VirtualAllocations()
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


    std::cout << "press enter to free\n";
    [[maybe_unused]] auto const ch1 = getchar();

    HeapFree(heap, 0x0, virtual_allocations[0]);
    std::cout << "press enter to exit\n";
    [[maybe_unused]] auto const ch2 = getchar();

    return EXIT_SUCCESS;
}