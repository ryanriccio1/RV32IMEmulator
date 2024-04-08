#pragma once
namespace RV32IM
{
	template <typename T, size_t sample_size>
	class MovingAverage
	{
	public:
		MovingAverage();
		void add_sample(T sample);
		T get_average() const;

	private:
		T samples[sample_size];
		size_t sample_idx;

		T sum;
	};

	template <typename T, size_t sample_size>
	MovingAverage<T, sample_size>::MovingAverage(): samples{}, sample_idx(sample_size - 1), sum(0) {}
	

	template <typename T, size_t sample_size>
	void MovingAverage<T, sample_size>::add_sample(T sample)
	{
		sum -= samples[sample_idx];
		samples[sample_idx] = sample;
		sum += sample;
		if (sample_idx == 0)
			sample_idx = sample_size;
		sample_idx--;
	}

	template <typename T, size_t sample_size>
	T MovingAverage<T, sample_size>::get_average() const
	{
		return T(sum / sample_size);
	}
}
