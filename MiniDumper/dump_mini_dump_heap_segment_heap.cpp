#include "dump_file_options.h"

#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/heap_lfh_affinity_slot.h"
#include "DbgHelpUtils/heap_lfh_bucket.h"
#include "DbgHelpUtils/heap_lfh_context.h"
#include "DbgHelpUtils/heap_lfh_entry.h"
#include "DbgHelpUtils/heap_lfh_subsegment.h"
#include "DbgHelpUtils/heap_page_segment.h"
#include "DbgHelpUtils/heap_segment_context.h"
#include "DbgHelpUtils/heap_vs_context.h"
#include "DbgHelpUtils/heap_vs_entry.h"
#include "DbgHelpUtils/heap_vs_subsegment.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/large_alloc_entry.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/page_range_descriptor.h"
#include "DbgHelpUtils/page_range_flags_utils.h"
#include "DbgHelpUtils/segment_heap.h"
#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std;
using namespace dlg_help_utils;

namespace detail
{
    namespace
    {
        void print_heap_large_entry_single_line(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
        {
            using namespace size_units::base_16;
            log << std::format(L"{0:{1}}Large Entry: {2} Size({3}) VirtualPtr({4}) UnusedBytes({5}) UserPtr({6}) ReqSize({7})\n", L' ', indent
                , stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length)
                , to_wstring(entry.size())
                , stream_hex_dump::to_hex(entry.virtual_address(), hex_length)
                , to_wstring(entry.unused_bytes())
                , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                , to_wstring(entry.user_requested_size()));
        }

        void print_heap_large_entry_debug(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}Large Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));
            log << std::format(L"{0}  Virtual Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.virtual_address(), hex_length));
            log << std::format(L"{0}  Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.unused_bytes()), stream_hex_dump::to_hex(entry.unused_bytes()));
            log << std::format(L"{0}  Spare: {1}\n", indent_str, stream_hex_dump::to_hex(entry.spare()));
            log << std::format(L"{0}  Extra Present: {1}\n", indent_str, entry.extra_present());
            log << std::format(L"{0}  Guard Page Count: {1}\n", indent_str, locale_formatting::to_wstring(entry.guard_page_count()));
            log << std::format(L"{0}  Guard Page Alignment: {1}\n", indent_str, locale_formatting::to_wstring(entry.guard_page_alignment()));
            log << std::format(L"{0}  Allocated Pages: {1}\n", indent_str, locale_formatting::to_wstring(entry.allocated_pages()));
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
            log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
            log << std::format(L"{0}  Has Front Padding: {1}\n", indent_str, entry.front_padding_size().has_value());
            if(auto const& front_padding_size = entry.front_padding_size();
                front_padding_size.has_value())
            {
                log << std::format(L"{0}  Front Padding Size: {1} ({2})\n", indent_str, locale_formatting::to_wstring(front_padding_size.value()), stream_hex_dump::to_hex(front_padding_size.value()));
            }
            log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
        }

        void print_heap_large_entry(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, dump_file_options const& options, size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_heap_large_entry_debug(log, hex_length, entry, indent);
            }
            else
            {
                print_heap_large_entry_single_line(log, hex_length, entry, indent);
            }

            if(options.display_symbols() && !entry.allocation_stack_trace().empty())
            {
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                dump_stack_to_stream(log, entry.heap().walker(), entry.allocation_stack_trace(), stream_stack_dump::is_x86_target_t{entry.heap().peb().is_x86_target()}, indent + 4);
                log << L'\n';
            }

            if(options.hex_dump_memory_data())
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                    log << L'\n';
                }
            }
        }

        void print_lfh_entry_single_line(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
        {
            using namespace size_units::base_16;
            log << std::format(L"{0:{1}}{2} LFH Entry: {3} BlockSize({4}){5} UserPtr({6}) ReqSize({7})\n", L' ', indent
                , entry.allocated() ? L"Busy"sv : L"Free"sv
                , stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length)
                , to_wstring(entry.block_size())
                , entry.has_unused_bytes() ? std::format(L" UnusedBytes({})", to_wstring(entry.unused_bytes())) : L""s
                , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                , to_wstring(entry.user_requested_size()));
        }

        void print_lfh_entry_debug(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}LFH Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length));
            log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(entry.block_size()), stream_hex_dump::to_hex(entry.block_size()));
            log << std::format(L"{0}  Allocated: {1}\n", indent_str, entry.allocated());
            log << std::format(L"{0}  Has Unused Bytes: {1}\n", indent_str, entry.has_unused_bytes());
            if(entry.has_unused_bytes())
            {
                log << std::format(L"{0}  Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.unused_bytes()), stream_hex_dump::to_hex(entry.unused_bytes()));
            }
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
            log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
            log << std::format(L"{0}  Has Front Padding: {1}\n", indent_str, entry.front_padding_size().has_value());
            if(auto const& front_padding_size = entry.front_padding_size();
                front_padding_size.has_value())
            {
                log << std::format(L"{0}  Front Padding Size: {1} ({2})\n", indent_str, locale_formatting::to_wstring(front_padding_size.value()), stream_hex_dump::to_hex(front_padding_size.value()));
            }
            if(entry.ust_address() != 0)
            {
                log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
            }
        }

        void print_lfh_entry(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, dump_file_options const& options, size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_lfh_entry_debug(log, hex_length, entry, indent);
            }
            else
            {
                print_lfh_entry_single_line(log, hex_length, entry, indent);
            }

            if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
            {
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                dump_stack_to_stream(log, entry.heap().walker(), entry.allocation_stack_trace(), stream_stack_dump::is_x86_target_t{entry.heap().peb().is_x86_target()}, indent + 4);
                log << L'\n';
            }

            if(options.hex_dump_memory_data() && entry.allocated() && entry.user_address() != 0)
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                    log << L'\n';
                }
            }
        }

        void print_lfh_subsegment(std::wostream& log, streamsize const hex_length, heap::heap_lfh_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}LFH Subsegment: {1}\n", indent_str, stream_hex_dump::to_hex(subsegment.heap_lfh_subsegment_address(), hex_length));
            log << std::format(L"{0}  Free Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.free_count()));
            log << std::format(L"{0}  Block Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.block_count()));
            log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(subsegment.block_size()), stream_hex_dump::to_hex(subsegment.block_size()));
            auto const location{subsegment.location()};
            log << std::format(L"{0}  Location: {1} ({2})\n", indent_str, locale_formatting::to_wstring(static_cast<uint16_t>(location)), dump_location_to_string(location));
            if(options.debug_heap_data())
            {
                log << std::format(L"{0}  Witheld Block Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.witheld_block_count()));
                log << std::format(L"{0}  Commit Unit Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.commit_unit_count()));
                log << std::format(L"{0}  Commit Unit Shift: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.commit_unit_shift()));
            }

            for(auto const& entry : subsegment.entries())
            {
                print_lfh_entry(log, hex_length, entry, options, indent + 2);
            }
        }

        void print_lfh_affinity_slot(std::wostream& log, streamsize const hex_length, heap::heap_lfh_affinity_slot const& affinity_slot, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}LFH Affinity Slot: {1}\n", indent_str, stream_hex_dump::to_hex(affinity_slot.heap_lfh_affinity_slot_address(), hex_length));
            log << std::format(L"{0}  Slot Index: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.slot_index()));
            log << std::format(L"{0}  Bucket Index: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.bucket_index()));
            log << std::format(L"{0}  Available Subsegment Count: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.available_subsegment_count()));

            for(auto const& subsegment : affinity_slot.subsegments())
            {
                print_lfh_subsegment(log, hex_length, subsegment, options, indent + 2);
            }
        }

        void print_lfh_bucket_single_line(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
        {
            using namespace size_units::base_16;

            std::wstring bucket_address;
            if(lfh_bucket.is_enabled())
            {
                bucket_address = std::format(L"{} @ ", stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length));
            }

            log << std::format(L"{0:{1}}LFH Bucket: {2}{3} {4} Granularity({5}) MaxAllocationSize({6})"
                , L' '
                , indent
                , bucket_address
                , locale_formatting::to_wstring(lfh_bucket.bucket_index())
                , lfh_bucket.is_enabled() ? L"Enabled"sv : L"Disabled"sv
                , to_wstring(lfh_bucket.bucket_granularity())
                , to_wstring(lfh_bucket.max_allocation_size()));

            if(lfh_bucket.is_enabled())
            {
                log << std::format(L" TotalBlockCount({0}) TotalSubsegmentCount({1}) Shift({2})"
                                                          , locale_formatting::to_wstring(lfh_bucket.total_block_count())
                                                          , locale_formatting::to_wstring(lfh_bucket.total_subsegment_count())
                                                          , locale_formatting::to_wstring(lfh_bucket.shift()));
            }
            else
            {
                log << std::format(L" UsageCount({})", locale_formatting::to_wstring(lfh_bucket.usage_count()));
            }
            log << L'\n';
        }

        void print_lfh_bucket_debug(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}LFH Bucket: {1}\n", indent_str, lfh_bucket.is_enabled() ? stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length) : std::wstring{});
            log << std::format(L"{0}  Bucket Index: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.bucket_index()));
            log << std::format(L"{0}  Bucket Enabled: {1}\n", indent_str, lfh_bucket.is_enabled());
            log << std::format(L"{0}  Granularity: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.bucket_granularity()), stream_hex_dump::to_hex(lfh_bucket.bucket_granularity()));
            log << std::format(L"{0}  Max Allocation Size: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.max_allocation_size()), stream_hex_dump::to_hex(lfh_bucket.max_allocation_size()));

            if(lfh_bucket.is_enabled())
            {
                log << std::format(L"{0}  Slot Count: {1}\n", indent_str, lfh_bucket.slot_count());
                log << std::format(L"{0}  Total Block Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.total_block_count()));
                log << std::format(L"{0}  Total Subsegment Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.total_subsegment_count()));
                log << std::format(L"{0}  Shift: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.shift()));
                log << std::format(L"{0}  Reciprocal Block Size: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.reciprocal_block_size()), stream_hex_dump::to_hex(lfh_bucket.reciprocal_block_size()));
                log << std::format(L"{}  Processor Affinity Mapping:\n", indent_str);
                auto const processor_affinity_mapping = lfh_bucket.processor_affinity_mapping();
                for(size_t index = 0; index < processor_affinity_mapping.size(); ++index)
                {
                    log << std::format(L"{0}    {{ CPU:{1} -> Slot:{2} }}\n", indent_str, locale_formatting::to_wstring(index), locale_formatting::to_wstring(processor_affinity_mapping[index]));
                }
            }
            else
            {
                log << std::format(L"{0}  Usage Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.usage_count()));
                log << std::format(L"{0}  Bucket Raw Data: {1}\n", indent_str, stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address_raw(), hex_length));
            }
        }

        void print_lfh_bucket(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, dump_file_options const& options, size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_lfh_bucket_debug(log, hex_length, lfh_bucket, indent);
            }
            else
            {
                print_lfh_bucket_single_line(log, hex_length, lfh_bucket, indent);
            }

            if(lfh_bucket.is_enabled())
            {
                for(auto const& affinity_slot : lfh_bucket.affinity_slots())
                {
                    print_lfh_affinity_slot(log, hex_length, affinity_slot, options, indent + 2);
                }
            }
        }

        void print_lfh_context_heap(std::wostream& log, streamsize const hex_length, const heap::heap_lfh_context& lfh_context, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}LFH Context: {1}\n", indent_str, stream_hex_dump::to_hex(lfh_context.heap_lfh_context_address(), hex_length));
            log << std::format(L"{0}  Max Affinity: {1}\n", indent_str, locale_formatting::to_wstring(lfh_context.max_affinity()));
            log << std::format(L"{0}  Max Block Size: {1} ({2})\n", indent_str, to_wstring(lfh_context.max_block_size()), stream_hex_dump::to_hex(lfh_context.max_block_size()));
            log << std::format(L"{0}  With Old Page Crossing Blocks: {1}\n", indent_str, lfh_context.with_old_page_crossing_blocks());
            log << std::format(L"{0}  Disable Randomization: {1}\n", indent_str, lfh_context.disable_randomization());

            for(auto const& bucket : lfh_context.active_buckets())
            {
                print_lfh_bucket(log, hex_length, bucket, options, indent + 2);
            }
        }

        std::wstring_view get_vs_entry_title(heap::heap_vs_entry const& entry)
        {
            return (entry.uncommitted_range() ? L"Uncommitted"sv : (entry.allocated() ? L"Busy"sv : L"Free"sv));
        }

        void print_vs_entry_single_line(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
        {
            using namespace size_units::base_16;
            log << std::format(L"{0:{1}}VS {2} Entry: {3} size({4} - {5})", L' ', indent
                , get_vs_entry_title(entry)
                , stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length)
                , to_wstring(entry.size())
                , stream_hex_dump::to_hex(entry.size()));

            if(!entry.uncommitted_range())
            {
                log << std::format(L" PSize({})" , to_wstring(entry.previous_size()));
                if(entry.has_unused_bytes())
                {
                    log << std::format(L" UnusedBytes({})", to_wstring(entry.unused_bytes()));
                }
                log << std::format(L" UserPtr({0}) ReqSize({1})"
                    , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                    , to_wstring(entry.user_requested_size()));
                if(!entry.is_valid())
                {
                    log << L" - invalid";
                }
            }

            log << L'\n';
        }

        void print_vs_entry_debug(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}VS {1} Entry: {2}\n", indent_str
                , get_vs_entry_title(entry)
                , stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));

            if(!entry.uncommitted_range())
            {
                log << std::format(L"{0}  Previous Size: {1} ({2})\n", indent_str, to_wstring(entry.previous_size()), stream_hex_dump::to_hex(entry.previous_size()));
                log << std::format(L"{0}  Is Valid: {1}\n", indent_str, entry.is_valid());
                log << std::format(L"{0}  Allocated: {1}\n", indent_str, entry.allocated());
                log << std::format(L"{0}  Memory Cost: {1}\n", indent_str, locale_formatting::to_wstring(entry.memory_cost()));
                log << std::format(L"{0}  Segment Page Offset: {1}\n", indent_str, locale_formatting::to_wstring(entry.segment_page_offset()));
                log << std::format(L"{0}  Has Unused Bytes: {1}\n", indent_str, entry.has_unused_bytes());
                if(entry.has_unused_bytes())
                {
                    log << std::format(L"{0}  Unused Bytes: {1}\n", indent_str, to_wstring(entry.unused_bytes()));
                }
                log << std::format(L"{0}  Skip During Walk: {1}\n", indent_str, entry.skip_during_walk());
                log << std::format(L"{0}  Spare: {1}\n", indent_str, stream_hex_dump::to_hex(entry.spare()));
                log << std::format(L"{0}  Allocated Chunk Bits: {1}\n", indent_str, stream_hex_dump::to_hex(entry.allocated_chunk_bits()));
                log << std::format(L"{0}  Has Front Padding: {1}\n", indent_str, entry.front_padding_size().has_value());
                if(auto const& front_padding_size = entry.front_padding_size();
                    front_padding_size.has_value())
                {
                    log << std::format(L"{0}  Front Padding Size: {1} ({2})\n", indent_str, locale_formatting::to_wstring(front_padding_size.value()), stream_hex_dump::to_hex(front_padding_size.value()));
                }
                log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
                log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
                log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
            }
        }

        void print_vs_entry(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, dump_file_options const& options, size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_vs_entry_debug(log, hex_length, entry, indent);
            }
            else
            {
                print_vs_entry_single_line(log, hex_length, entry, indent);
            }

            if(!entry.uncommitted_range())
            {
                if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
                {
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                dump_stack_to_stream(log, entry.heap().walker(), entry.allocation_stack_trace(), stream_stack_dump::is_x86_target_t{entry.heap().peb().is_x86_target()}, indent + 4);
                    log << L'\n';
                }

                if(options.hex_dump_memory_data() && entry.allocated())
                {
                    uint64_t const size = entry.user_requested_size().count();
                    if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                    {
                        hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                        log << L'\n';
                    }
                }
            }
        }

        void print_vs_subsegment(std::wostream& log, streamsize const hex_length, heap::heap_vs_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}VS Subsegment: {1}\n", indent_str, stream_hex_dump::to_hex(subsegment.heap_vs_subsegment_address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(subsegment.size()), stream_hex_dump::to_hex(subsegment.size()));
            log << std::format(L"{0}  Signature: {1}{2}\n", indent_str, stream_hex_dump::to_hex(subsegment.signature()), subsegment.is_signature_valid() ? L" (valid)"sv : L" (invalid)"sv);
            log << std::format(L"{0}  Full Commit: {1}\n", indent_str, subsegment.full_commit());

            for(auto const& entry : subsegment.entries())
            {
                print_vs_entry(log, hex_length, entry, options, indent + 2);
            }
        }

        void print_vs_context_heap(std::wostream& log, streamsize const hex_length, heap::heap_vs_context const& vs_context, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            log << std::format(L"{0}VS Context: {1}\n", indent_str, stream_hex_dump::to_hex(vs_context.heap_vs_context_address(), hex_length));
            log << std::format(L"{0}  Total Committed Units: {1}\n", indent_str, vs_context.total_committed_units());
            log << std::format(L"{0}  Free Committed Units: {1}\n", indent_str, vs_context.free_committed_units());

            for(auto const& subsegment : vs_context.subsegments())
            {
                print_vs_subsegment(log, hex_length, subsegment, options, indent + 2);
            }

            log << std::format(L"{0}  Free Entries:\n", indent_str);
            for(auto const& entry : vs_context.free_entries())
            {
                print_vs_entry(log, hex_length, entry, options, indent + 4);
            }
        }

        void print_segment_heap_page_entry_single_line(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
        {
            using namespace size_units::base_16;
            auto const range_flags = entry.range_flags();
            log << std::format(L"{0:{1}}Page Entry: {2} : {3} Extra({4}) BlockPtr({5}) BlockSize({6}) UserPtr({7}) ReqSize({8}) Flags({9}) {10}\n", ' ', indent
                , locale_formatting::to_wstring(entry.index())
                , stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length)
                , to_wstring(entry.extra_bytes())
                , stream_hex_dump::to_hex(entry.block_address(), hex_length)
                , to_wstring(entry.block_size())
                , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                , to_wstring(entry.user_requested_size())
                , stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags))
                , dump_page_range_to_string(range_flags));
        }

        void print_segment_heap_page_entry_debug(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            auto const range_flags = entry.range_flags();
            log << std::format(L"{0}Page Entry: {1} : {2}\n", indent_str, locale_formatting::to_wstring(entry.index()), stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length));
            log << std::format(L"{0}  Extra Present: {1}\n", indent_str, entry.extra_present());
            log << std::format(L"{0}  Range Flags: {1} ({2}) - {3}\n", indent_str, stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags)), dump_page_range_flags_to_string(range_flags), dump_page_range_to_string(range_flags));
            log << std::format(L"{0}  Extra Bytes: {1} ({2})\n", indent_str, to_wstring(entry.extra_bytes()), stream_hex_dump::to_hex(entry.extra_bytes()));
            log << std::format(L"{0}  Committed Page Count: {1}\n", indent_str, locale_formatting::to_wstring(entry.committed_page_count()));
            log << std::format(L"{0}  Unit Offset: {1}\n", indent_str, locale_formatting::to_wstring(entry.unit_offset()));
            log << std::format(L"{0}  Unit Size: {1} ({2})\n", indent_str, to_wstring(entry.unit_size()), stream_hex_dump::to_hex(entry.unit_size()));
            log << std::format(L"{0}  Block Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.block_address(), hex_length));
            log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(entry.block_size()), stream_hex_dump::to_hex(entry.block_size()));
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
            log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
            log << std::format(L"{0}  Has Front Padding: {1}\n", indent_str, entry.front_padding_size().has_value());
            if(auto const& front_padding_size = entry.front_padding_size();
                front_padding_size.has_value())
            {
                log << std::format(L"{0}  Front Padding Size: {1} ({2})\n", indent_str, locale_formatting::to_wstring(front_padding_size.value()), stream_hex_dump::to_hex(front_padding_size.value()));
            }
            log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
        }

        void print_segment_heap_page_entry(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, dump_file_options const& options, size_t const indent)
        {
            if(options.debug_heap_data())
            {
                print_segment_heap_page_entry_debug(log, hex_length, entry, indent);
            }
            else
            {
                print_segment_heap_page_entry_single_line(log, hex_length, entry, indent);
            }

            if(options.display_symbols() && !entry.allocation_stack_trace().empty())
            {
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                dump_stack_to_stream(log, entry.heap().walker(), entry.allocation_stack_trace(), stream_stack_dump::is_x86_target_t{entry.heap().peb().is_x86_target()}, indent + 4);
                log << L'\n';
            }

            if(options.hex_dump_memory_data() && entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT && entry.user_address() != 0)
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 4);
                    log << L'\n';
                }
            }
        }

        void print_segment_heap_page(std::wostream& log, streamsize const hex_length, size_t const page_index, heap::heap_page_segment const& page, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}Segment Page: {1} : {2}\n" , indent_str, locale_formatting::to_wstring(page_index), stream_hex_dump::to_hex(page.heap_page_segment_address(), hex_length));
            log << std::format(L"{0}  Signature: {1}{2}\n", indent_str, stream_hex_dump::to_hex(page.signature(), hex_length), page.is_signature_valid() ? L" (valid)"sv : L""sv);

            if(options.debug_heap_data())
            {
                for(auto const& entry : page.all_entries())
                {
                    print_segment_heap_page_entry(log, hex_length, entry, options, indent + 2);
                }
            }
            else
            {
                for(auto const& entry : page.entries())
                {
                    print_segment_heap_page_entry(log, hex_length, entry, options, indent + 2);
                }
            }
        }

        void print_segment_context_heap(std::wostream& log, streamsize const hex_length, size_t const segment_context_index, heap::heap_segment_context const& segment_context, dump_file_options const& options, size_t const indent)
        {
            std::wstring const indent_str(indent, L' ');
            using namespace size_units::base_16;
            log << std::format(L"{0}Segment Context: {1} : {2}\n", indent_str, locale_formatting::to_wstring(segment_context_index), stream_hex_dump::to_hex(segment_context.heap_segment_context_address(), hex_length));
            log << std::format(L"{0}  Segment Mask: {1}\n", indent_str, stream_hex_dump::to_hex(segment_context.segment_mask(), hex_length));
            log << std::format(L"{0}  Max Allocation Size: {1} ({2})\n", indent_str, to_wstring(segment_context.max_allocation_size()), stream_hex_dump::to_hex(segment_context.max_allocation_size()));
            log << std::format(L"{0}  Unit Shift: {1}\n", indent_str, stream_hex_dump::to_hex(segment_context.unit_shift()));
            log << std::format(L"{0}  Pages Per Unit Shift: {1}\n", indent_str, locale_formatting::to_wstring(segment_context.pages_per_unit_shift()));
            log << std::format(L"{0}  Segment Count: {1}\n", indent_str, locale_formatting::to_wstring(segment_context.segment_count()));

            size_t index{0};
            for(auto const& page : segment_context.pages())
            {
                print_segment_heap_page(log, hex_length, index, page, options, indent + 2);
                ++index;
            }

            auto first = true;
            for(auto const& entry : segment_context.free_page_ranges())
            {
                if(first)
                {
                    log << std::format(L"{0}  Free Page Ranges:\n", indent_str);
                    first = false;
                }
                print_segment_heap_page_entry(log, hex_length, entry, options, indent + 4);
            }
        }
    }

    void print_segment_heap_line(std::wostream& log, std::wstring_view const& process_heap_marker, heap::segment_heap const& segment_heap)
    {
        using namespace size_units::base_16;
        log << std::format(L" Segment Heap {0} {2:<{1}} {3:<{1}} {4:<{1}} {5:<{1}} {6}\n"
            , stream_hex_dump::to_hex_full(segment_heap.global_flags())
            , units_max_width
            , to_wstring(segment_heap.reserved())
            , to_wstring(segment_heap.committed())
            , to_wstring(segment_heap.uncommitted())
            , to_wstring(segment_heap.large_reserved())
            , process_heap_marker);
    }

    void print_segment_heap(std::wostream& log, std::streamsize const hex_length, std::wstring_view const& process_heap_marker, heap::segment_heap const& segment_heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Segment Heap: {1} {2}\n", indent_str, stream_hex_dump::to_hex(segment_heap.segment_heap_address(), hex_length), process_heap_marker);
        log << std::format(L"{0}  Global Flags: {1}\n", indent_str, stream_hex_dump::to_hex_full(segment_heap.global_flags()));
        log << std::format(L"{0}  Total Reserved: {1} ({2})\n", indent_str, to_wstring(segment_heap.reserved()), stream_hex_dump::to_hex(segment_heap.reserved()));
        log << std::format(L"{0}  Total Committed: {1} ({2})\n", indent_str, to_wstring(segment_heap.committed()), stream_hex_dump::to_hex(segment_heap.committed()));
        log << std::format(L"{0}  Total Uncommitted: {1} ({2})\n", indent_str, to_wstring(segment_heap.uncommitted()), stream_hex_dump::to_hex(segment_heap.uncommitted()));
        log << std::format(L"{0}  Large Reserved: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_reserved()), stream_hex_dump::to_hex(segment_heap.large_reserved()));
        log << std::format(L"{0}  Large Committed: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_committed()), stream_hex_dump::to_hex(segment_heap.large_committed()));
        log << std::format(L"{0}  Large Uncommitted: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_uncommitted()), stream_hex_dump::to_hex(segment_heap.large_uncommitted()));
        log << std::format(L"{0}  Total Reserved Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.total_reserved_pages()));
        log << std::format(L"{0}  Total Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.total_committed_pages()));
        log << std::format(L"{0}  Free Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.free_committed_pages()));
        log << std::format(L"{0}  LFH Free Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.lfh_free_committed_pages()));
        log << std::format(L"{0}  Large Reserved Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.large_reserved_pages()));
        log << std::format(L"{0}  Large Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.large_committed_pages()));
        log << std::format(L"{0}  Heap Key: {1}\n", indent_str, stream_hex_dump::to_hex(segment_heap.heap_key(), hex_length));
        log << L'\n';

        size_t index{0};
        for(auto const& segment_context : segment_heap.segment_contexts())
        {
            print_segment_context_heap(log, hex_length, index, segment_context, options, indent + 2);
            ++index;
        }

        print_vs_context_heap(log, hex_length, segment_heap.vs_context(), options, indent + 2);
        print_lfh_context_heap(log, hex_length, segment_heap.lfh_context(), options, indent + 2);

        for(auto const& entry : segment_heap.large_entries())
        {
            print_heap_large_entry(log, hex_length, entry, options, indent + 2);
        }
    }
}
