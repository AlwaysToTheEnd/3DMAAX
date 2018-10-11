#pragma once

template<typename ValueType>
class cPercentageValueSet
{
private:
	template<typename Type>
	struct PerValue
	{
		float weight=0;
		Type value;
	};

public:
	cPercentageValueSet();
	~cPercentageValueSet() {}

	void AddValue(float weight, ValueType value);
	ValueType GetRandomValue();
	ValueType GetGreaterValueThanBenchmark(float benchValue);
	ValueType GetLowerValueThanBenchmark(float benchValue);

private:
	double						m_totalWeight;
	vector<PerValue<ValueType>>	m_perValues;
	mt19937						m_random;
};

template<typename ValueType>
inline cPercentageValueSet<ValueType>::cPercentageValueSet()
	: m_totalWeight(0)
	, m_random(42)
{

}

template<typename ValueType>
inline void cPercentageValueSet<ValueType>::AddValue(float weight, ValueType value)
{
	m_perValues.push_back({ weight , value });
	m_totalWeight += weight;

	sort(m_perValues.begin(), m_perValues.end(),[](PerValue<ValueType>& lhs, PerValue<ValueType>& rhs)
		{
			return lhs.weight >= rhs.weight;
		}
	);
}

template<typename ValueType>
inline ValueType cPercentageValueSet<ValueType>::GetRandomValue()
{
	return ValueType();
}

template<typename ValueType>
inline ValueType cPercentageValueSet<ValueType>::GetGreaterValueThanBenchmark(float benchValue)
{
	return ValueType();
}

template<typename ValueType>
inline ValueType cPercentageValueSet<ValueType>::GetLowerValueThanBenchmark(float benchValue)
{
	return ValueType();
}
