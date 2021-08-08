// AllocationSetupTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <iostream>
#include <Windows.h>

int main()
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

    std::cout << "press any key to free\n";
    [[maybe_unused]] auto const ch1 = getchar();

    HeapFree(heap, 0x0, backend_allocations[0]);
    HeapFree(heap, 0x0, frontend_allocations[0]);
    std::cout << "press any key to exit\n";
    [[maybe_unused]] auto const ch2 = getchar();

    return EXIT_SUCCESS;
}
