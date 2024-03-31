﻿#include "exception_utils.h"

#include "windows_setup.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> g_exception_code_descriptions =
    {
        // bug check code reference: https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-code-reference2
        {0x00000001, L"APC_INDEX_MISMATCH"s},
        {0x00000002, L"DEVICE_QUEUE_NOT_BUSY"s},
        {0x00000003, L"INVALID_AFFINITY_SET"s},
        {0x00000004, L"INVALID_DATA_ACCESS_TRAP"s},
        {0x00000005, L"INVALID_PROCESS_ATTACH_ATTEMPT"s},
        {0x00000006, L"INVALID_PROCESS_DETACH_ATTEMPT"s},
        {0x00000007, L"INVALID_SOFTWARE_INTERRUPT"s},
        {0x00000008, L"IRQL_NOT_DISPATCH_LEVEL"s},
        {0x00000009, L"IRQL_NOT_GREATER_OR_EQUAL"s},
        {0x0000000A, L"IRQL_NOT_LESS_OR_EQUAL"s},
        {0x0000000B, L"NO_EXCEPTION_HANDLING_SUPPORT"s},
        {0x0000000C, L"MAXIMUM_WAIT_OBJECTS_EXCEEDED"s},
        {0x0000000D, L"MUTEX_LEVEL_NUMBER_VIOLATION"s},
        {0x0000000E, L"NO_USER_MODE_CONTEXT"s},
        {0x0000000F, L"SPIN_LOCK_ALREADY_OWNED"s},
        {0x00000010, L"SPIN_LOCK_NOT_OWNED"s},
        {0x00000011, L"THREAD_NOT_MUTEX_OWNER"s},
        {0x00000012, L"TRAP_CAUSE_UNKNOWN"s},
        {0x00000013, L"EMPTY_THREAD_REAPER_LIST"s},
        {0x00000014, L"CREATE_DELETE_LOCK_NOT_LOCKED"s},
        {0x00000015, L"LAST_CHANCE_CALLED_FROM_KMODE"s},
        {0x00000016, L"CID_HANDLE_CREATION"s},
        {0x00000017, L"CID_HANDLE_DELETION"s},
        {0x00000018, L"REFERENCE_BY_POINTER"s},
        {0x00000019, L"BAD_POOL_HEADER"s},
        {0x0000001A, L"MEMORY_MANAGEMENT"s},
        {0x0000001B, L"PFN_SHARE_COUNT"s},
        {0x0000001C, L"PFN_REFERENCE_COUNT"s},
        {0x0000001D, L"NO_SPIN_LOCK_AVAILABLE"s},
        {0x0000001E, L"KMODE_EXCEPTION_NOT_HANDLED"s},
        {0x0000001F, L"SHARED_RESOURCE_CONV_ERROR"s},
        {0x00000020, L"KERNEL_APC_PENDING_DURING_EXIT"s},
        {0x00000021, L"QUOTA_UNDERFLOW"s},
        {0x00000022, L"FILE_SYSTEM"s},
        {0x00000023, L"FAT_FILE_SYSTEM"s},
        {0x00000024, L"NTFS_FILE_SYSTEM"s},
        {0x00000025, L"NPFS_FILE_SYSTEM"s},
        {0x00000026, L"CDFS_FILE_SYSTEM"s},
        {0x00000027, L"RDR_FILE_SYSTEM"s},
        {0x00000028, L"CORRUPT_ACCESS_TOKEN"s},
        {0x00000029, L"SECURITY_SYSTEM"s},
        {0x0000002A, L"INCONSISTENT_IRP"s},
        {0x0000002B, L"PANIC_STACK_SWITCH"s},
        {0x0000002C, L"PORT_DRIVER_INTERNAL"s},
        {0x0000002D, L"SCSI_DISK_DRIVER_INTERNAL"s},
        {0x0000002E, L"DATA_BUS_ERROR"s},
        {0x0000002F, L"INSTRUCTION_BUS_ERROR"s},
        {0x00000030, L"SET_OF_INVALID_CONTEXT"s},
        {0x00000031, L"PHASE0_INITIALIZATION_FAILED"s},
        {0x00000032, L"PHASE1_INITIALIZATION_FAILED"s},
        {0x00000033, L"UNEXPECTED_INITIALIZATION_CALL"s},
        {0x00000034, L"CACHE_MANAGER"s},
        {0x00000035, L"NO_MORE_IRP_STACK_LOCATIONS"s},
        {0x00000036, L"DEVICE_REFERENCE_COUNT_NOT_ZERO"s},
        {0x00000037, L"FLOPPY_INTERNAL_ERROR"s},
        {0x00000038, L"SERIAL_DRIVER_INTERNAL"s},
        {0x00000039, L"SYSTEM_EXIT_OWNED_MUTEX"s},
        {0x0000003A, L"SYSTEM_UNWIND_PREVIOUS_USER"s},
        {0x0000003B, L"SYSTEM_SERVICE_EXCEPTION"s},
        {0x0000003C, L"INTERRUPT_UNWIND_ATTEMPTED"s},
        {0x0000003D, L"INTERRUPT_EXCEPTION_NOT_HANDLED"s},
        {0x0000003E, L"MULTIPROCESSOR_CONFIGURATION_NOT_SUPPORTED"s},
        {0x0000003F, L"NO_MORE_SYSTEM_PTES"s},
        {0x00000040, L"TARGET_MDL_TOO_SMALL"s},
        {0x00000041, L"MUST_SUCCEED_POOL_EMPTY"s},
        {0x00000042, L"ATDISK_DRIVER_INTERNAL"s},
        {0x00000043, L"NO_SUCH_PARTITION"s},
        {0x00000044, L"MULTIPLE_IRP_COMPLETE_REQUESTS"s},
        {0x00000045, L"INSUFFICIENT_SYSTEM_MAP_REGS"s},
        {0x00000046, L"DEREF_UNKNOWN_LOGON_SESSION"s},
        {0x00000047, L"REF_UNKNOWN_LOGON_SESSION"s},
        {0x00000048, L"CANCEL_STATE_IN_COMPLETED_IRP"s},
        {0x00000049, L"PAGE_FAULT_WITH_INTERRUPTS_OFF"s},
        {0x0000004A, L"IRQL_GT_ZERO_AT_SYSTEM_SERVICE"s},
        {0x0000004B, L"STREAMS_INTERNAL_ERROR"s},
        {0x0000004C, L"FATAL_UNHANDLED_HARD_ERROR"s},
        {0x0000004D, L"NO_PAGES_AVAILABLE"s},
        {0x0000004E, L"PFN_LIST_CORRUPT"s},
        {0x0000004F, L"NDIS_INTERNAL_ERROR"s},
        {0x00000050, L"PAGE_FAULT_IN_NONPAGED_AREA"s},
        {0x00000051, L"REGISTRY_ERROR"s},
        {0x00000052, L"MAILSLOT_FILE_SYSTEM"s},
        {0x00000053, L"NO_BOOT_DEVICE"s},
        {0x00000054, L"LM_SERVER_INTERNAL_ERROR"s},
        {0x00000055, L"DATA_COHERENCY_EXCEPTION"s},
        {0x00000056, L"INSTRUCTION_COHERENCY_EXCEPTION"s},
        {0x00000057, L"XNS_INTERNAL_ERROR"s},
        {0x00000058, L"FTDISK_INTERNAL_ERROR"s},
        {0x00000059, L"PINBALL_FILE_SYSTEM"s},
        {0x0000005A, L"CRITICAL_SERVICE_FAILED"s},
        {0x0000005B, L"SET_ENV_VAR_FAILED"s},
        {0x0000005C, L"HAL_INITIALIZATION_FAILED"s},
        {0x0000005D, L"UNSUPPORTED_PROCESSOR"s},
        {0x0000005E, L"OBJECT_INITIALIZATION_FAILED"s},
        {0x0000005F, L"SECURITY_INITIALIZATION_FAILED"s},
        {0x00000060, L"PROCESS_INITIALIZATION_FAILED"s},
        {0x00000061, L"HAL1_INITIALIZATION_FAILED"s},
        {0x00000062, L"OBJECT1_INITIALIZATION_FAILED"s},
        {0x00000063, L"SECURITY1_INITIALIZATION_FAILED"s},
        {0x00000064, L"SYMBOLIC_INITIALIZATION_FAILED"s},
        {0x00000065, L"MEMORY1_INITIALIZATION_FAILED"s},
        {0x00000066, L"CACHE_INITIALIZATION_FAILED"s},
        {0x00000067, L"CONFIG_INITIALIZATION_FAILED"s},
        {0x00000068, L"FILE_INITIALIZATION_FAILED"s},
        {0x00000069, L"IO1_INITIALIZATION_FAILED"s},
        {0x0000006A, L"LPC_INITIALIZATION_FAILED"s},
        {0x0000006B, L"PROCESS1_INITIALIZATION_FAILED"s},
        {0x0000006C, L"REFMON_INITIALIZATION_FAILED"s},
        {0x0000006D, L"SESSION1_INITIALIZATION_FAILED"s},
        {0x0000006E, L"SESSION2_INITIALIZATION_FAILED"s},
        {0x0000006F, L"SESSION3_INITIALIZATION_FAILED"s},
        {0x00000070, L"SESSION4_INITIALIZATION_FAILED"s},
        {0x00000071, L"SESSION5_INITIALIZATION_FAILED"s},
        {0x00000072, L"ASSIGN_DRIVE_LETTERS_FAILED"s},
        {0x00000073, L"CONFIG_LIST_FAILED"s},
        {0x00000074, L"BAD_SYSTEM_CONFIG_INFO"s},
        {0x00000075, L"CANNOT_WRITE_CONFIGURATION"s},
        {0x00000076, L"PROCESS_HAS_LOCKED_PAGES"s},
        {0x00000077, L"KERNEL_STACK_INPAGE_ERROR"s},
        {0x00000078, L"PHASE0_EXCEPTION"s},
        {0x00000079, L"MISMATCHED_HAL"s},
        {0x0000007A, L"KERNEL_DATA_INPAGE_ERROR"s},
        {0x0000007B, L"INACCESSIBLE_BOOT_DEVICE"s},
        {0x0000007C, L"BUGCODE_NDIS_DRIVER"s},
        {0x0000007D, L"INSTALL_MORE_MEMORY"s},
        {0x0000007E, L"SYSTEM_THREAD_EXCEPTION_NOT_HANDLED"s},
        {0x0000007F, L"UNEXPECTED_KERNEL_MODE_TRAP"s},
        {0x00000080, L"NMI_HARDWARE_FAILURE"s},
        {0x00000081, L"SPIN_LOCK_INIT_FAILURE"s},
        {0x00000082, L"DFS_FILE_SYSTEM"s},
        {0x00000085, L"SETUP_FAILURE"s},
        {0x0000008B, L"MBR_CHECKSUM_MISMATCH"s},
        {0x0000008E, L"KERNEL_MODE_EXCEPTION_NOT_HANDLED"s},
        {0x0000008F, L"PP0_INITIALIZATION_FAILED"s},
        {0x00000090, L"PP1_INITIALIZATION_FAILED"s},
        {0x00000092, L"UP_DRIVER_ON_MP_SYSTEM"s},
        {0x00000093, L"INVALID_KERNEL_HANDLE"s},
        {0x00000094, L"KERNEL_STACK_LOCKED_AT_EXIT"s},
        {0x00000096, L"INVALID_WORK_QUEUE_ITEM"s},
        {0x00000097, L"BOUND_IMAGE_UNSUPPORTED"s},
        {0x00000098, L"END_OF_NT_EVALUATION_PERIOD"s},
        {0x00000099, L"INVALID_REGION_OR_SEGMENT"s},
        {0x0000009A, L"SYSTEM_LICENSE_VIOLATION"s},
        {0x0000009B, L"UDFS_FILE_SYSTEM"s},
        {0x0000009C, L"MACHINE_CHECK_EXCEPTION"s},
        {0x0000009E, L"USER_MODE_HEALTH_MONITOR"s},
        {0x0000009F, L"DRIVER_POWER_STATE_FAILURE"s},
        {0x000000A0, L"INTERNAL_POWER_ERROR"s},
        {0x000000A1, L"PCI_BUS_DRIVER_INTERNAL"s},
        {0x000000A2, L"MEMORY_IMAGE_CORRUPT"s},
        {0x000000A3, L"ACPI_DRIVER_INTERNAL"s},
        {0x000000A4, L"CNSS_FILE_SYSTEM_FILTER"s},
        {0x000000A5, L"ACPI_BIOS_ERROR"s},
        {0x000000A7, L"BAD_EXHANDLE"s},
        {0x000000AB, L"SESSION_HAS_VALID_POOL_ON_EXIT"s},
        {0x000000AC, L"HAL_MEMORY_ALLOCATION"s},
        {0x000000AD, L"VIDEO_DRIVER_DEBUG_REPORT_REQUEST"s},
        {0x000000B1, L"BGI_DETECTED_VIOLATION"s},
        {0x000000B4, L"VIDEO_DRIVER_INIT_FAILURE"s},
        {0x000000B8, L"ATTEMPTED_SWITCH_FROM_DPC"s},
        {0x000000B9, L"CHIPSET_DETECTED_ERROR"s},
        {0x000000BA, L"SESSION_HAS_VALID_VIEWS_ON_EXIT"s},
        {0x000000BB, L"NETWORK_BOOT_INITIALIZATION_FAILED"s},
        {0x000000BC, L"NETWORK_BOOT_DUPLICATE_ADDRESS"s},
        {0x000000BD, L"INVALID_HIBERNATED_STATE"s},
        {0x000000BE, L"ATTEMPTED_WRITE_TO_READONLY_MEMORY"s},
        {0x000000BF, L"MUTEX_ALREADY_OWNED"s},
        {0x000000C1, L"SPECIAL_POOL_DETECTED_MEMORY_CORRUPTION"s},
        {0x000000C2, L"BAD_POOL_CALLER"s},
        {0x000000C4, L"DRIVER_VERIFIER_DETECTED_VIOLATION"s},
        {0x000000C5, L"DRIVER_CORRUPTED_EXPOOL"s},
        {0x000000C6, L"DRIVER_CAUGHT_MODIFYING_FREED_POOL"s},
        {0x000000C7, L"TIMER_OR_DPC_INVALID"s},
        {0x000000C8, L"IRQL_UNEXPECTED_VALUE"s},
        {0x000000C9, L"DRIVER_VERIFIER_IOMANAGER_VIOLATION"s},
        {0x000000CA, L"PNP_DETECTED_FATAL_ERROR"s},
        {0x000000CB, L"DRIVER_LEFT_LOCKED_PAGES_IN_PROCESS"s},
        {0x000000CC, L"PAGE_FAULT_IN_FREED_SPECIAL_POOL"s},
        {0x000000CD, L"PAGE_FAULT_BEYOND_END_OF_ALLOCATION"s},
        {0x000000CE, L"DRIVER_UNLOADED_WITHOUT_CANCELLING_PENDING_OPERATIONS"s},
        {0x000000CF, L"TERMINAL_SERVER_DRIVER_MADE_INCORRECT_MEMORY_REFERENCE"s},
        {0x000000D0, L"DRIVER_CORRUPTED_MMPOOL"s},
        {0x000000D1, L"DRIVER_IRQL_NOT_LESS_OR_EQUAL"s},
        {0x000000D2, L"BUGCODE_ID_DRIVER"s},
        {0x000000D3, L"DRIVER_PORTION_MUST_BE_NONPAGED"s},
        {0x000000D4, L"SYSTEM_SCAN_AT_RAISED_IRQL_CAUGHT_IMPROPER_DRIVER_UNLOAD"s},
        {0x000000D5, L"DRIVER_PAGE_FAULT_IN_FREED_SPECIAL_POOL"s},
        {0x000000D6, L"DRIVER_PAGE_FAULT_BEYOND_END_OF_ALLOCATION"s},
        {0x000000D7, L"DRIVER_UNMAPPING_INVALID_VIEW"s},
        {0x000000D8, L"DRIVER_USED_EXCESSIVE_PTES"s},
        {0x000000D9, L"LOCKED_PAGES_TRACKER_CORRUPTION"s},
        {0x000000DA, L"SYSTEM_PTE_MISUSE"s},
        {0x000000DB, L"DRIVER_CORRUPTED_SYSPTES"s},
        {0x000000DC, L"DRIVER_INVALID_STACK_ACCESS"s},
        {0x000000DE, L"POOL_CORRUPTION_IN_FILE_AREA"s},
        {0x000000DF, L"IMPERSONATING_WORKER_THREAD"s},
        {0x000000E0, L"ACPI_BIOS_FATAL_ERROR"s},
        {0x000000E1, L"WORKER_THREAD_RETURNED_AT_BAD_IRQL"s},
        {0x000000E2, L"MANUALLY_INITIATED_CRASH"s},
        {0x000000E3, L"RESOURCE_NOT_OWNED"s},
        {0x000000E4, L"WORKER_INVALID"s},
        {0x000000E6, L"DRIVER_VERIFIER_DMA_VIOLATION"s},
        {0x000000E7, L"INVALID_FLOATING_POINT_STATE"s},
        {0x000000E8, L"INVALID_CANCEL_OF_FILE_OPEN"s},
        {0x000000E9, L"ACTIVE_EX_WORKER_THREAD_TERMINATION"s},
        {0x000000EA, L"THREAD_STUCK_IN_DEVICE_DRIVER"s},
        {0x000000EB, L"DIRTY_MAPPED_PAGES_CONGESTION"s},
        {0x000000EC, L"SESSION_HAS_VALID_SPECIAL_POOL_ON_EXIT"s},
        {0x000000ED, L"UNMOUNTABLE_BOOT_VOLUME"s},
        {0x000000EF, L"CRITICAL_PROCESS_DIED"s},
        {0x000000F0, L"STORAGE_MINIPORT_ERROR"s},
        {0x000000F1, L"SCSI_VERIFIER_DETECTED_VIOLATION"s},
        {0x000000F2, L"HARDWARE_INTERRUPT_STORM"s},
        {0x000000F3, L"DISORDERLY_SHUTDOWN"s},
        {0x000000F4, L"CRITICAL_OBJECT_TERMINATION"s},
        {0x000000F5, L"FLTMGR_FILE_SYSTEM"s},
        {0x000000F6, L"PCI_VERIFIER_DETECTED_VIOLATION"s},
        {0x000000F7, L"DRIVER_OVERRAN_STACK_BUFFER"s},
        {0x000000F8, L"RAMDISK_BOOT_INITIALIZATION_FAILED"s},
        {0x000000F9, L"DRIVER_RETURNED_STATUS_REPARSE_FOR_VOLUME_OPEN"s},
        {0x000000FA, L"HTTP_DRIVER_CORRUPTED"s},
        {0x000000FC, L"ATTEMPTED_EXECUTE_OF_NOEXECUTE_MEMORY"s},
        {0x000000FD, L"DIRTY_NOWRITE_PAGES_CONGESTION"s},
        {0x000000FE, L"BUGCODE_USB_DRIVER"s},
        {0x000000FF, L"RESERVE_QUEUE_OVERFLOW"s},
        {0x00000100, L"LOADER_BLOCK_MISMATCH"s},
        {0x00000101, L"CLOCK_WATCHDOG_TIMEOUT"s},
        {0x00000102, L"DPC_WATCHDOG_TIMEOUT"s},
        {0x00000103, L"MUP_FILE_SYSTEM"s},
        {0x00000104, L"AGP_INVALID_ACCESS"s},
        {0x00000105, L"AGP_GART_CORRUPTION"s},
        {0x00000106, L"AGP_ILLEGALLY_REPROGRAMMED"s},
        {0x00000108, L"THIRD_PARTY_FILE_SYSTEM_FAILURE"s},
        {0x00000109, L"CRITICAL_STRUCTURE_CORRUPTION"s},
        {0x0000010A, L"APP_TAGGING_INITIALIZATION_FAILED"s},
        {0x0000010C, L"FSRTL_EXTRA_CREATE_PARAMETER_VIOLATION"s},
        {0x0000010D, L"WDF_VIOLATION"s},
        {0x0000010E, L"VIDEO_MEMORY_MANAGEMENT_INTERNAL"s},
        {0x0000010F, L"RESOURCE_MANAGER_EXCEPTION_NOT_HANDLED"s},
        {0x00000111, L"RECURSIVE_NMI"s},
        {0x00000112, L"MSRPC_STATE_VIOLATION"s},
        {0x00000113, L"VIDEO_DXGKRNL_FATAL_ERROR"s},
        {0x00000114, L"VIDEO_SHADOW_DRIVER_FATAL_ERROR"s},
        {0x00000115, L"AGP_INTERNAL"s},
        {0x00000116, L"VIDEO_TDR_FAILURE"s},
        {0x00000117, L"VIDEO_TDR_TIMEOUT_DETECTED"s},
        {0x00000119, L"VIDEO_SCHEDULER_INTERNAL_ERROR"s},
        {0x0000011A, L"EM_INITIALIZATION_FAILURE"s},
        {0x0000011B, L"DRIVER_RETURNED_HOLDING_CANCEL_LOCK"s},
        {0x0000011C, L"ATTEMPTED_WRITE_TO_CM_PROTECTED_STORAGE"s},
        {0x0000011D, L"EVENT_TRACING_FATAL_ERROR"s},
        {0x0000011E, L"TOO_MANY_RECURSIVE_FAULTS"s},
        {0x0000011F, L"INVALID_DRIVER_HANDLE"s},
        {0x00000120, L"BITLOCKER_FATAL_ERROR"s},
        {0x00000121, L"DRIVER_VIOLATION"s},
        {0x00000122, L"WHEA_INTERNAL_ERROR"s},
        {0x00000123, L"CRYPTO_SELF_TEST_FAILURE"s},
        {0x00000124, L"WHEA_UNCORRECTABLE_ERROR"s},
        {0x00000125, L"NMR_INVALID_STATE"s},
        {0x00000126, L"NETIO_INVALID_POOL_CALLER"s},
        {0x00000127, L"PAGE_NOT_ZERO"s},
        {0x00000128, L"WORKER_THREAD_RETURNED_WITH_BAD_IO_PRIORITY"s},
        {0x00000129, L"WORKER_THREAD_RETURNED_WITH_BAD_PAGING_IO_PRIORITY"s},
        {0x0000012A, L"MUI_NO_VALID_SYSTEM_LANGUAGE"s},
        {0x0000012B, L"FAULTY_HARDWARE_CORRUPTED_PAGE"s},
        {0x0000012C, L"EXFAT_FILE_SYSTEM"s},
        {0x0000012D, L"VOLSNAP_OVERLAPPED_TABLE_ACCESS"s},
        {0x0000012E, L"INVALID_MDL_RANGE"s},
        {0x0000012F, L"VHD_BOOT_INITIALIZATION_FAILED"s},
        {0x00000130, L"DYNAMIC_ADD_PROCESSOR_MISMATCH"s},
        {0x00000131, L"INVALID_EXTENDED_PROCESSOR_STATE"s},
        {0x00000132, L"RESOURCE_OWNER_POINTER_INVALID"s},
        {0x00000133, L"DPC_WATCHDOG_VIOLATION"s},
        {0x00000134, L"DRIVE_EXTENDER"s},
        {0x00000135, L"REGISTRY_FILTER_DRIVER_EXCEPTION"s},
        {0x00000136, L"VHD_BOOT_HOST_VOLUME_NOT_ENOUGH_SPACE"s},
        {0x00000137, L"WIN32K_HANDLE_MANAGER"s},
        {0x00000138, L"GPIO_CONTROLLER_DRIVER_ERROR"s},
        {0x00000139, L"KERNEL_SECURITY_CHECK_FAILURE"s},
        {0x0000013A, L"KERNEL_MODE_HEAP_CORRUPTION"s},
        {0x0000013B, L"PASSIVE_INTERRUPT_ERROR"s},
        {0x0000013C, L"INVALID_IO_BOOST_STATE"s},
        {0x0000013D, L"CRITICAL_INITIALIZATION_FAILURE"s},
        {0x00000140, L"STORAGE_DEVICE_ABNORMALITY_DETECTED"s},
        {0x00000141, L"VIDEO_ENGINE_TIMEOUT_DETECTED"s},
        {0x00000142, L"VIDEO_TDR_APPLICATION_BLOCKED"s},
        {0x00000143, L"PROCESSOR_DRIVER_INTERNAL"s},
        {0x00000144, L"BUGCODE_USB3_DRIVER"s},
        {0x00000145, L"SECURE_BOOT_VIOLATION"s},
        {0x00000147, L"ABNORMAL_RESET_DETECTED"s},
        {0x00000149, L"REFS_FILE_SYSTEM"s},
        {0x0000014A, L"KERNEL_WMI_INTERNAL"s},
        {0x0000014B, L"SOC_SUBSYSTEM_FAILURE"s},
        {0x0000014C, L"FATAL_ABNORMAL_RESET_ERROR"s},
        {0x0000014D, L"EXCEPTION_SCOPE_INVALID"s},
        {0x0000014E, L"SOC_CRITICAL_DEVICE_REMOVED"s},
        {0x0000014F, L"PDC_WATCHDOG_TIMEOUT"s},
        {0x00000150, L"TCPIP_AOAC_NIC_ACTIVE_REFERENCE_LEAK"s},
        {0x00000151, L"UNSUPPORTED_INSTRUCTION_MODE"s},
        {0x00000152, L"INVALID_PUSH_LOCK_FLAGS"s},
        {0x00000153, L"KERNEL_LOCK_ENTRY_LEAKED_ON_THREAD_TERMINATION"s},
        {0x00000154, L"UNEXPECTED_STORE_EXCEPTION"s},
        {0x00000155, L"OS_DATA_TAMPERING"s},
        {0x00000156, L"WINSOCK_DETECTED_HUNG_CLOSESOCKET_LIVEDUMP"s},
        {0x00000157, L"KERNEL_THREAD_PRIORITY_FLOOR_VIOLATION"s},
        {0x00000158, L"ILLEGAL_IOMMU_PAGE_FAULT"s},
        {0x00000159, L"HAL_ILLEGAL_IOMMU_PAGE_FAULT"s},
        {0x0000015A, L"SDBUS_INTERNAL_ERROR"s},
        {0x0000015B, L"WORKER_THREAD_RETURNED_WITH_SYSTEM_PAGE_PRIORITY_ACTIVE"s},
        {0x0000015C, L"PDC_WATCHDOG_TIMEOUT_LIVEDUMP"s},
        {0x0000015D, L"SOC_SUBSYSTEM_FAILURE_LIVEDUMP"s},
        {0x0000015E, L"BUGCODE_NDIS_DRIVER_LIVE_DUMP"s},
        {0x0000015F, L"CONNECTED_STANDBY_WATCHDOG_TIMEOUT_LIVEDUMP"s},
        {0x00000160, L"WIN32K_ATOMIC_CHECK_FAILURE"s},
        {0x00000161, L"LIVE_SYSTEM_DUMP"s},
        {0x00000162, L"KERNEL_AUTO_BOOST_INVALID_LOCK_RELEASE"s},
        {0x00000163, L"WORKER_THREAD_TEST_CONDITION"s},
        {0x00000164, L"WIN32K_CRITICAL_FAILURE"s},
        {0x00000165, L"CLUSTER_CSV_STATUS_IO_TIMEOUT_LIVEDUMP"s},
        {0x00000166, L"CLUSTER_RESOURCE_CALL_TIMEOUT_LIVEDUMP"s},
        {0x00000167, L"CLUSTER_CSV_SNAPSHOT_DEVICE_INFO_TIMEOUT_LIVEDUMP"s},
        {0x00000168, L"CLUSTER_CSV_STATE_TRANSITION_TIMEOUT_LIVEDUMP"s},
        {0x00000169, L"CLUSTER_CSV_VOLUME_ARRIVAL_LIVEDUMP"s},
        {0x0000016A, L"CLUSTER_CSV_VOLUME_REMOVAL_LIVEDUMP"s},
        {0x0000016B, L"CLUSTER_CSV_CLUSTER_WATCHDOG_LIVEDUMP"s},
        {0x0000016C, L"INVALID_RUNDOWN_PROTECTION_FLAGS"s},
        {0x0000016D, L"INVALID_SLOT_ALLOCATOR_FLAGS"s},
        {0x0000016E, L"ERESOURCE_INVALID_RELEASE"s},
        {0x0000016F, L"CLUSTER_CSV_STATE_TRANSITION_INTERVAL_TIMEOUT_LIVEDUMP"s},
        {0x00000170, L"CRYPTO_LIBRARY_INTERNAL_ERROR"s},
        {0x00000171, L"CLUSTER_CSV_CLUSSVC_DISCONNECT_WATCHDOG"s},
        {0x00000173, L"COREMSGCALL_INTERNAL_ERROR"s},
        {0x00000174, L"COREMSG_INTERNAL_ERROR"s},
        {0x00000175, L"PREVIOUS_FATAL_ABNORMAL_RESET_ERROR"s},
        {0x00000178, L"ELAM_DRIVER_DETECTED_FATAL_ERROR"s},
        {0x00000179, L"CLUSTER_CLUSPORT_STATUS_IO_TIMEOUT_LIVEDUMP"s},
        {0x0000017B, L"PROFILER_CONFIGURATION_ILLEGAL"s},
        {0x0000017C, L"PDC_LOCK_WATCHDOG_LIVEDUMP"s},
        {0x0000017D, L"PDC_UNEXPECTED_REVOCATION_LIVEDUMP"s},
        {0x0000017E, L"MICROCODE_REVISION_MISMATCH"s},
        {0x00000187, L"VIDEO_DWMINIT_TIMEOUT_FALLBACK_BDD"s},
        {0x00000188, L"CLUSTER_CSVFS_LIVEDUMP"s},
        {0x00000189, L"BAD_OBJECT_HEADER"s},
        {0x0000018B, L"SECURE_KERNEL_ERROR"s},
        {0x0000018C, L"HYPERGUARD_VIOLATION"s},
        {0x0000018D, L"SECURE_FAULT_UNHANDLED"s},
        {0x0000018E, L"KERNEL_PARTITION_REFERENCE_VIOLATION"s},
        {0x00000190, L"WIN32K_CRITICAL_FAILURE_LIVEDUMP"s},
        {0x00000191, L"PF_DETECTED_CORRUPTION"s},
        {0x00000192, L"KERNEL_AUTO_BOOST_LOCK_ACQUISITION_WITH_RAISED_IRQL"s},
        {0x00000193, L"VIDEO_DXGKRNL_LIVEDUMP"s},
        {0x00000195, L"SMB_SERVER_LIVEDUMP"s},
        {0x00000196, L"LOADER_ROLLBACK_DETECTED"s},
        {0x00000197, L"WIN32K_SECURITY_FAILURE"s},
        {0x00000198, L"UFX_LIVEDUMP"s},
        {0x00000199, L"KERNEL_STORAGE_SLOT_IN_USE"s},
        {0x0000019A, L"WORKER_THREAD_RETURNED_WHILE_ATTACHED_TO_SILO"s},
        {0x0000019B, L"TTM_FATAL_ERROR"s},
        {0x0000019C, L"WIN32K_POWER_WATCHDOG_TIMEOUT"s},
        {0x0000019D, L"CLUSTER_SVHDX_LIVEDUMP"s},
        {0x000001A0, L"TTM_WATCHDOG_TIMEOUT"s},
        {0x000001A1, L"WIN32K_CALLOUT_WATCHDOG_LIVEDUMP"s},
        {0x000001A2, L"WIN32K_CALLOUT_WATCHDOG_BUGCHECK"s},
        {0x000001A3, L"CALL_HAS_NOT_RETURNED_WATCHDOG_TIMEOUT_LIVEDUMP"s},
        {0x000001A4, L"DRIPS_SW_HW_DIVERGENCE_LIVEDUMP"s},
        {0x000001A5, L"USB_DRIPS_BLOCKER_SURPRISE_REMOVAL_LIVEDUMP"s},
        {0x000001A6, L"BLUETOOTH_ERROR_RECOVERY_LIVEDUMP"s},
        {0x000001A7, L"SMB_REDIRECTOR_LIVEDUMP"s},
        {0x000001A8, L"VIDEO_DXGKRNL_BLACK_SCREEN_LIVEDUMP"s},
        {0x000001B0, L"VIDEO_MINIPORT_FAILED_LIVEDUMP"s},
        {0x000001B8, L"VIDEO_MINIPORT_BLACK_SCREEN_LIVEDUMP"s},
        {0x000001C4, L"DRIVER_VERIFIER_DETECTED_VIOLATION_LIVEDUMP"s},
        {0x000001C5, L"IO_THREADPOOL_DEADLOCK_LIVEDUMP"s},
        {0x000001C6, L"FAST_ERESOURCE_PRECONDITION_VIOLATION"s},
        {0x000001C7, L"STORE_DATA_STRUCTURE_CORRUPTION"s},
        {0x000001C8, L"MANUALLY_INITIATED_POWER_BUTTON_HOLD"s},
        {0x000001C9, L"USER_MODE_HEALTH_MONITOR_LIVEDUMP"s},
        {0x000001CA, L"SYNTHETIC_WATCHDOG_TIMEOUT"s},
        {0x000001CB, L"INVALID_SILO_DETACH"s},
        {0x000001CC, L"EXRESOURCE_TIMEOUT_LIVEDUMP"s},
        {0x000001CD, L"INVALID_CALLBACK_STACK_ADDRESS"s},
        {0x000001CE, L"INVALID_KERNEL_STACK_ADDRESS"s},
        {0x000001CF, L"HARDWARE_WATCHDOG_TIMEOUT"s},
        {0x000001D0, L"CPI_FIRMWARE_WATCHDOG_TIMEOUT"s},
        {0x000001D1, L"TELEMETRY_ASSERTS_LIVEDUMP"s},
        {0x000001D2, L"WORKER_THREAD_INVALID_STATE"s},
        {0x000001D3, L"WFP_INVALID_OPERATION"s},
        {0x000001D4, L"UCMUCSI_LIVEDUMP"s},
        {0x000001D5, L"DRIVER_PNP_WATCHDOG"s},
        {0x000001D6, L"WORKER_THREAD_RETURNED_WITH_NON_DEFAULT_WORKLOAD_CLASS"s},
        {0x000001D7, L"EFS_FATAL_ERROR"s},
        {0x000001D8, L"UCMUCSI_FAILURE"s},
        {0x000001D9, L"HAL_IOMMU_INTERNAL_ERROR"s},
        {0x000001DA, L"HAL_BLOCKED_PROCESSOR_INTERNAL_ERROR"s},
        {0x000001DB, L"IPI_WATCHDOG_TIMEOUT"s},
        {0x000001DC, L"DMA_COMMON_BUFFER_VECTOR_ERROR"s},
        {0x00000356, L"XBOX_ERACTRL_CS_TIMEOUT"s},
        {0x00000BFE, L"BC_BLUETOOTH_VERIFIER_FAULT"s},
        {0x00000BFF, L"BC_BTHMINI_VERIFIER_FAULT"s},
        {0x00020001, L"HYPERVISOR_ERROR"s},
        {0x1000007E, L"SYSTEM_THREAD_EXCEPTION_NOT_HANDLED_M"s},
        {0x1000007F, L"UNEXPECTED_KERNEL_MODE_TRAP_M"s},
        {0x1000008E, L"KERNEL_MODE_EXCEPTION_NOT_HANDLED_M"s},
        {0x100000EA, L"THREAD_STUCK_IN_DEVICE_DRIVER_M"s},
        {0x4000008A, L"THREAD_TERMINATE_HELD_MUTEX"s},
        {0xC0000218, L"STATUS_CANNOT_LOAD_REGISTRY_FILE"s},
        {0xC000021A, L"WINLOGON_FATAL_ERROR"s},
        {0xC0000221, L"STATUS_IMAGE_CHECKSUM_MISMATCH"s},
        {0xDEADDEAD, L"MANUALLY_INITIATED_CRASH1"s},

        // win32 exceptions
        {static_cast<uint32_t>(EXCEPTION_ACCESS_VIOLATION), L"EXCEPTION_ACCESS_VIOLATION"s},
        {static_cast<uint32_t>(EXCEPTION_ARRAY_BOUNDS_EXCEEDED), L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED"s},
        {static_cast<uint32_t>(EXCEPTION_BREAKPOINT), L"EXCEPTION_BREAKPOINT"s},
        {static_cast<uint32_t>(EXCEPTION_DATATYPE_MISALIGNMENT), L"EXCEPTION_DATATYPE_MISALIGNMENT"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_DENORMAL_OPERAND), L"EXCEPTION_FLT_DENORMAL_OPERAND"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_DIVIDE_BY_ZERO), L"EXCEPTION_FLT_DIVIDE_BY_ZERO"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_INEXACT_RESULT), L"EXCEPTION_FLT_INEXACT_RESULT"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_INVALID_OPERATION), L"EXCEPTION_FLT_INVALID_OPERATION"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_OVERFLOW), L"EXCEPTION_FLT_OVERFLOW"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_STACK_CHECK), L"EXCEPTION_FLT_STACK_CHECK"s},
        {static_cast<uint32_t>(EXCEPTION_FLT_UNDERFLOW), L"EXCEPTION_FLT_UNDERFLOW"s},
        {static_cast<uint32_t>(EXCEPTION_ILLEGAL_INSTRUCTION), L"EXCEPTION_ILLEGAL_INSTRUCTION"s},
        {static_cast<uint32_t>(EXCEPTION_IN_PAGE_ERROR), L"EXCEPTION_IN_PAGE_ERROR"s},
        {static_cast<uint32_t>(EXCEPTION_INT_DIVIDE_BY_ZERO), L"EXCEPTION_INT_DIVIDE_BY_ZERO"s},
        {static_cast<uint32_t>(EXCEPTION_INT_OVERFLOW), L"EXCEPTION_INT_OVERFLOW"s},
        {static_cast<uint32_t>(EXCEPTION_INVALID_DISPOSITION), L"EXCEPTION_INVALID_DISPOSITION"s},
        {static_cast<uint32_t>(EXCEPTION_NONCONTINUABLE_EXCEPTION), L"EXCEPTION_NONCONTINUABLE_EXCEPTION"s},
        {static_cast<uint32_t>(EXCEPTION_PRIV_INSTRUCTION), L"EXCEPTION_PRIV_INSTRUCTION"s},
        {static_cast<uint32_t>(EXCEPTION_SINGLE_STEP), L"EXCEPTION_SINGLE_STEP"s},
        {static_cast<uint32_t>(EXCEPTION_STACK_OVERFLOW), L"EXCEPTION_STACK_OVERFLOW"s},
        {static_cast<uint32_t>(EXCEPTION_GUARD_PAGE), L"EXCEPTION_GUARD_PAGE"s},
        {static_cast<uint32_t>(EXCEPTION_INVALID_HANDLE), L"EXCEPTION_INVALID_HANDLE"s},
        {static_cast<uint32_t>(EXCEPTION_POSSIBLE_DEADLOCK), L"EXCEPTION_POSSIBLE_DEADLOCK"s},
        {static_cast<uint32_t>(CONTROL_C_EXIT), L"CONTROL_C_EXIT"s},
    };
}

namespace dlg_help_utils::exception_utils::resources
{
    std::unordered_map<uint32_t, std::wstring> const& get_exception_code_descriptions()
    {
        return g_exception_code_descriptions;
    }

    std::wstring_view get_unknown_exception_string()
    {
        return L"unknown exception"sv;
    }

    std::wstring_view get_exception_continuable(bool const exception_continuable)
    {
        return exception_continuable ? L"continuable exception"sv : L"non-continuable exception"sv;
    }

    std::wstring_view get_exception_unwinding(bool const exception_unwinding)
    {
        return exception_unwinding ? L", unwind is in progress"sv : L""sv;
    }

    std::wstring_view get_exception_exit_unwind(bool const exception_exit_unwind)
    {
        return exception_exit_unwind ? L", exit unwind is in progress"sv : L""sv;
    }

    std::wstring_view get_exception_stack_invalid(bool const exception_stack_invalid)
    {
        return exception_stack_invalid ? L", stack out of limits or unaligned"sv : L""sv;
    }

    std::wstring_view get_exception_nesting_level(bool const exception_nesting_level)
    {
        return exception_nesting_level ? L", nested exception handler call"sv : L""sv;
    }

    std::wstring_view get_exception_target_unwind(bool const exception_target_unwind)
    {
        return exception_target_unwind ? L", target unwind in progress"sv : L""sv;
    }

    std::wstring_view get_exception_collided_unwind(bool const exception_collided_unwind)
    {
        return exception_collided_unwind ? L", collided exception handler call"sv : L""sv;
    }
}
