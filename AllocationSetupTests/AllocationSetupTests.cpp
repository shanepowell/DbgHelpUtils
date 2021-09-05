// AllocationSetupTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 26812 26495)
#include <boost/program_options.hpp>
#pragma warning(pop)

int LfhAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator);
int VirtualAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator);
int AllocateSizeRanges(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator);
template<size_t N>
bool AllocateBuffers(std::wostream& log, std::function<void*(size_t size)> const& allocator, std::array<void*, N>& allocations, char& fill_value, size_t allocation_size, size_t increase_amount, char const* type);
template<size_t N>
void DeallocateSomeBuffers(std::wostream& log, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations);

template<size_t N, typename ...Args>
void DeallocateSomeBuffers(std::wostream& log, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations, Args... args)
{
    DeallocateSomeBuffers(log, deallocator, allocations);
    DeallocateSomeBuffers(log, deallocator, args...);
}

void CreateOutput(std::wostream& log, std::wstring const& dump_filename);
void GenerateDumpFile(std::wostream& log, std::wstring const& dump_filename);


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
                ("sizes", "generate range allocation sizes allocations")
                ("useheapalloc", "use HeapAlloc/HeapFree")
                ("usemalloc", "use malloc/free")
                ("usenew", "use new/delete")
                ("dmp", po::wvalue<std::wstring>(), "dump filename")
                ("log", po::wvalue<std::wstring>(), "log filename")
                ;

            po::variables_map vm;
            store(parse_command_line(argc, argv, options), vm);
            notify(vm);

            auto const do_lfh_allocations = vm.count("lfh") != 0;
            auto const do_virtual_allocations = vm.count("virtual") != 0;
            auto const do_sizes = vm.count("sizes") != 0;
            auto const use_malloc = vm.count("usemalloc") != 0;
            auto const use_new = vm.count("usenew") != 0;
            auto const dump_filename = vm["dmp"].as<std::wstring>();
            auto const log_filename = vm["log"].as<std::wstring>();

            if (vm.count("help") || (!do_lfh_allocations && !do_virtual_allocations && !do_sizes))
            {
                std::cout << options << "\n";
                return EXIT_SUCCESS;
            }

            std::function allocator = [](size_t const size) { return HeapAlloc(GetProcessHeap(), 0x0, size); };
            std::function deallocator = [](void* memory) { HeapFree(GetProcessHeap(), 0x0, memory); };

            if(use_malloc)
            {
                allocator = [](size_t const size) { return malloc(size); };
                deallocator = [](void* memory) { free(memory); };
            }

            if(use_new)
            {
                allocator = [](size_t const size) { return new char[size]; };
                deallocator = [](void* memory) { delete[] static_cast<char*>(memory); };
            }

            std::unique_ptr<std::wfstream> log;
            std::wostream* o_log{&std::wcout};
            if(!log_filename.empty())
            {
                log = std::make_unique<std::wfstream>(log_filename, std::ios_base::out | std::ios_base::trunc);
                if(log->bad())
                {
                    std::wcout << "failed to open " << log_filename << '\n';
                    return EXIT_FAILURE;
                }
                o_log = log.get();
            }

            auto result = EXIT_SUCCESS;
            if(do_lfh_allocations)
            {
                result = LfhAllocations(*o_log, dump_filename, allocator, deallocator);
            }

            if(do_virtual_allocations)
            {
                result = VirtualAllocations(*o_log, dump_filename, allocator, deallocator);
            }

            if(do_sizes)
            {
                result = AllocateSizeRanges(*o_log, dump_filename, allocator, deallocator);
            }

            log->close();
            return result;
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

int LfhAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator)
{
    std::array<void*, 0x12> backend_allocations{};
    constexpr size_t allocation_size = 0x10;

    char fill_value = 'A';
    if (!AllocateBuffers(log, allocator, backend_allocations, fill_value, allocation_size, 0, "backend"))
    {
        return EXIT_FAILURE;
    }

    std::array<void*, 0x12> frontend_allocations{};
    if (!AllocateBuffers(log, allocator, frontend_allocations, fill_value, allocation_size, 0, "frontend"))
    {
        return EXIT_FAILURE;
    }

    DeallocateSomeBuffers(log, deallocator, backend_allocations, frontend_allocations);

    CreateOutput(log, dump_filename);

    return EXIT_SUCCESS;
}


int VirtualAllocations(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator)
{
    std::array<void*, 0x5> virtual_allocations{};
    
    if (char fill_value = 'A';
        !AllocateBuffers(log, allocator, virtual_allocations, fill_value, 0x100000, 0, "virtual"))
    {
        return EXIT_FAILURE;
    }

    DeallocateSomeBuffers(log, deallocator, virtual_allocations);

    CreateOutput(log, dump_filename);

    return EXIT_SUCCESS;
}

int AllocateSizeRanges(std::wostream& log, std::wstring const& dump_filename, std::function<void*(size_t size)> const& allocator, std::function<void(void*)> const& deallocator)
{
    size_t allocation_size = 0x1;

    auto constexpr max_rounds = 6;

    std::array<std::array<void*, 0xF>, max_rounds> allocation_groups{};
    std::array<std::array<void*, 0xE>, max_rounds - 1> small_allocation_groups{};

    for(auto index = 0; index < max_rounds; ++index)
    {
        char fill_value = 'A';
        if (!AllocateBuffers(log, allocator, allocation_groups[index], fill_value, allocation_size, allocation_size, "sizes"))
        {
            return EXIT_FAILURE;
        }
        if(allocation_size > 1)
        {
            if (!AllocateBuffers(log, allocator, small_allocation_groups[index - 1], fill_value, allocation_size + 1, 1, "sizes"))
            {
                return EXIT_FAILURE;
            }
        }
        allocation_size *= 0x10;
    }

    for(auto& allocations : allocation_groups)
    {
        DeallocateSomeBuffers(log, deallocator, allocations);
    }

    for(auto& allocations : small_allocation_groups)
    {
        DeallocateSomeBuffers(log, deallocator, allocations);
    }

    CreateOutput(log, dump_filename);
    return EXIT_SUCCESS;
}

template<size_t N>
bool AllocateBuffers(std::wostream& log, std::function<void*(size_t size)> const& allocator, std::array<void*, N>& small_allocations, char& fill_value, size_t allocation_size, size_t const increase_amount, char const* type)
{
    for (auto& virtual_allocation : small_allocations)
    {
        virtual_allocation = allocator(allocation_size);
        log << type << " allocation (" << virtual_allocation <<  ") [" << fill_value << "] of buffer size [" << allocation_size << " | 0x" << std::hex << allocation_size << std::dec << "]\n";
        if(virtual_allocation == nullptr)
        {
            log << "Failed to allocate " << type << " allocation\n";
            return false;
        }
        memset(virtual_allocation, fill_value, allocation_size);
        ++fill_value;
        allocation_size += increase_amount;
    }
    return true;
}

template<size_t N>
void DeallocateSomeBuffers(std::wostream& log, std::function<void(void*)> const& deallocator, std::array<void*, N>& allocations)
{
    deallocator(allocations[1]);
    deallocator(allocations[3]);

    log << "deallocate [" << allocations[1] << "]\n";
    log << "deallocate [" << allocations[3] << "]\n";
}

void CreateOutput(std::wostream& log, std::wstring const& free_dump_filename)
{
    if(free_dump_filename.empty())
    {
        std::cout << "press enter to exit\n";
        [[maybe_unused]] auto const ch2 = getchar();
    }
    else
    {
        GenerateDumpFile(log, free_dump_filename);
    }
}

void GenerateDumpFile(std::wostream& log, std::wstring const& dump_filename)
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
        log << "Command line [" << command << "] to long : " << command.size() << " >= " << MAX_PATH << '\n';
        return;
    }

    wchar_t temp[MAX_PATH];
    wcsncpy_s(temp, command.c_str(), command.size());
    temp[command.size()] = 0;

    if (!CreateProcessW(nullptr, temp, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info))
    {
        log << "CreateProcessW [" << command << "] failed " << GetLastError() << '\n';
        return;
    }

    //Close thread handle. 
    CloseHandle(process_info.hThread);

    // Wait for dump to complete...
    WaitForSingleObject(process_info.hProcess, INFINITE);

    // Close process handle.
    CloseHandle(process_info.hProcess);

    log << "Dump file created: " << dump_filename << '\n';
}
