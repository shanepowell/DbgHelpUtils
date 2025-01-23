#pragma once

#include "M128A.g.h"

#include "DbgHelpUtils/windows_setup.h"
#include "GlobalOptionsNotifyPropertyChangedBase.h"


namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A : M128AT<M128A>, GlobalOptionsNotifyPropertyChangedBase<M128A>
    {
        M128A();

        Windows::Foundation::Collections::IObservableVector<uint8_t> UInt8() const { return uint8_; }
        uint8_t UInt8_0() const { return uint8_.GetAt(0); }
        uint8_t UInt8_1() const { return uint8_.GetAt(1); }
        uint8_t UInt8_2() const { return uint8_.GetAt(2); }
        uint8_t UInt8_3() const { return uint8_.GetAt(3); }
        uint8_t UInt8_4() const { return uint8_.GetAt(4); }
        uint8_t UInt8_5() const { return uint8_.GetAt(5); }
        uint8_t UInt8_6() const { return uint8_.GetAt(6); }
        uint8_t UInt8_7() const { return uint8_.GetAt(7); }
        uint8_t UInt8_8() const { return uint8_.GetAt(8); }
        uint8_t UInt8_9() const { return uint8_.GetAt(9); }
        uint8_t UInt8_10() const { return uint8_.GetAt(10); }
        uint8_t UInt8_11() const { return uint8_.GetAt(11); }
        uint8_t UInt8_12() const { return uint8_.GetAt(12); }
        uint8_t UInt8_13() const { return uint8_.GetAt(13); }
        uint8_t UInt8_14() const { return uint8_.GetAt(14); }
        uint8_t UInt8_15() const { return uint8_.GetAt(15); }

        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Int8() const { return int8_; }
        Windows::Foundation::IInspectable Int8_0() const { return int8_.GetAt(0); }
        Windows::Foundation::IInspectable Int8_1() const { return int8_.GetAt(1); }
        Windows::Foundation::IInspectable Int8_2() const { return int8_.GetAt(2); }
        Windows::Foundation::IInspectable Int8_3() const { return int8_.GetAt(3); }
        Windows::Foundation::IInspectable Int8_4() const { return int8_.GetAt(4); }
        Windows::Foundation::IInspectable Int8_5() const { return int8_.GetAt(5); }
        Windows::Foundation::IInspectable Int8_6() const { return int8_.GetAt(6); }
        Windows::Foundation::IInspectable Int8_7() const { return int8_.GetAt(7); }
        Windows::Foundation::IInspectable Int8_8() const { return int8_.GetAt(8); }
        Windows::Foundation::IInspectable Int8_9() const { return int8_.GetAt(9); }
        Windows::Foundation::IInspectable Int8_10() const { return int8_.GetAt(10); }
        Windows::Foundation::IInspectable Int8_11() const { return int8_.GetAt(11); }
        Windows::Foundation::IInspectable Int8_12() const { return int8_.GetAt(12); }
        Windows::Foundation::IInspectable Int8_13() const { return int8_.GetAt(13); }
        Windows::Foundation::IInspectable Int8_14() const { return int8_.GetAt(14); }
        Windows::Foundation::IInspectable Int8_15() const { return int8_.GetAt(15); }

        Windows::Foundation::Collections::IObservableVector<uint16_t> UInt16() const { return uint16_; }
        uint16_t UInt16_0() const { return uint16_.GetAt(0); }
        uint16_t UInt16_1() const { return uint16_.GetAt(1); }
        uint16_t UInt16_2() const { return uint16_.GetAt(2); }
        uint16_t UInt16_3() const { return uint16_.GetAt(3); }
        uint16_t UInt16_4() const { return uint16_.GetAt(4); }
        uint16_t UInt16_5() const { return uint16_.GetAt(5); }
        uint16_t UInt16_6() const { return uint16_.GetAt(6); }
        uint16_t UInt16_7() const { return uint16_.GetAt(7); }

        Windows::Foundation::Collections::IObservableVector<int16_t> Int16() const { return int16_; }
        int16_t Int16_0() const { return int16_.GetAt(0); }
        int16_t Int16_1() const { return int16_.GetAt(1); }
        int16_t Int16_2() const { return int16_.GetAt(2); }
        int16_t Int16_3() const { return int16_.GetAt(3); }
        int16_t Int16_4() const { return int16_.GetAt(4); }
        int16_t Int16_5() const { return int16_.GetAt(5); }
        int16_t Int16_6() const { return int16_.GetAt(6); }
        int16_t Int16_7() const { return int16_.GetAt(7); }

        Windows::Foundation::Collections::IObservableVector<uint32_t> UInt32() const { return uint32_; }
        uint32_t UInt32_0() const { return uint32_.GetAt(0); }
        uint32_t UInt32_1() const { return uint32_.GetAt(1); }
        uint32_t UInt32_2() const { return uint32_.GetAt(2); }
        uint32_t UInt32_3() const { return uint32_.GetAt(3); }

        Windows::Foundation::Collections::IObservableVector<int32_t> Int32() const { return int32_; }
        int32_t Int32_0() const { return int32_.GetAt(0); }
        int32_t Int32_1() const { return int32_.GetAt(1); }
        int32_t Int32_2() const { return int32_.GetAt(2); }
        int32_t Int32_3() const { return int32_.GetAt(3); }

        Windows::Foundation::Collections::IObservableVector<uint64_t> UInt64() const { return uint64_; }
        uint64_t UInt64_0() const { return uint64_.GetAt(0); }
        uint64_t UInt64_1() const { return uint64_.GetAt(1); }

        Windows::Foundation::Collections::IObservableVector<int64_t> Int64() const { return int64_; }
        int64_t Int64_0() const { return int64_.GetAt(0); }
        int64_t Int64_1() const { return int64_.GetAt(1); }

        Windows::Foundation::Collections::IObservableVector<float> Float32() const { return float32_; }
        float Float32_0() const { return float32_.GetAt(0); }
        float Float32_1() const { return float32_.GetAt(1); }
        float Float32_2() const { return float32_.GetAt(2); }
        float Float32_3() const { return float32_.GetAt(3); }

        Windows::Foundation::Collections::IObservableVector<double> Float64() const { return float64_; }
        double Float64_0() const { return float64_.GetAt(0); }
        double Float64_1() const { return float64_.GetAt(1); }

        void Set(_M128A const& m128a);

        _M128A const& Get() const { return m128a_; }

    private:
        _M128A m128a_{};
        Windows::Foundation::Collections::IObservableVector<uint8_t> uint8_{single_threaded_observable_vector<uint8_t>()};
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> int8_{single_threaded_observable_vector<Windows::Foundation::IInspectable>()};
        Windows::Foundation::Collections::IObservableVector<uint16_t> uint16_{single_threaded_observable_vector<uint16_t>()};
        Windows::Foundation::Collections::IObservableVector<int16_t> int16_{single_threaded_observable_vector<int16_t>()};
        Windows::Foundation::Collections::IObservableVector<uint32_t> uint32_{single_threaded_observable_vector<uint32_t>()};
        Windows::Foundation::Collections::IObservableVector<int32_t> int32_{single_threaded_observable_vector<int32_t>()};
        Windows::Foundation::Collections::IObservableVector<uint64_t> uint64_{single_threaded_observable_vector<uint64_t>()};
        Windows::Foundation::Collections::IObservableVector<int64_t> int64_{single_threaded_observable_vector<int64_t>()};
        Windows::Foundation::Collections::IObservableVector<float> float32_{single_threaded_observable_vector<float>()};
        Windows::Foundation::Collections::IObservableVector<double> float64_{single_threaded_observable_vector<double>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct M128A : M128AT<M128A, implementation::M128A>
    {
    };
}
