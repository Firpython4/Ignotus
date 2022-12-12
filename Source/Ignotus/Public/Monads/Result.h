#pragma once

template <typename OkType, typename ErrorType>
struct TResult
{
public:
	typedef OkType FOkType;
	typedef ErrorType FErrorType;

	TResult(const TResult& other)
		: _okValue(other._okValue),
		  _errorValue(other._errorValue)
	{
	}

	TResult(TResult&& other) noexcept
		: _okValue(other._okValue),
		  _errorValue(other._errorValue)
	{
	}

	TResult& operator=(const TResult& other)
	{
		if (this == &other)
		{
			return *this;
		}
		_okValue = other._okValue;
		_errorValue = other._errorValue;
		return *this;
	}

	TResult& operator=(TResult&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}
		_okValue = MoveTemp(other._okValue);
		_errorValue = MoveTemp(other._errorValue);
		return *this;
	}

	FORCEINLINE const TOptional<OkType>& GetOk() const
	{
		return _okValue;
	}

	FORCEINLINE const TOptional<ErrorType>& GetError() const
	{
		return _errorValue;
	}

	FORCEINLINE static TResult Ok(const OkType& okValue)
	{
		return TResult(okValue);
	}

	FORCEINLINE static TResult Error(const ErrorType& errorValue)
	{
		return TResult(errorValue);
	}

	FORCEINLINE static TResult Ok(OkType&& okValue) noexcept
	{
		return TResult(okValue);
	}

	FORCEINLINE static TResult Error(ErrorType&& errorValue) noexcept
	{
		return TResult(errorValue);
	}

	template <typename NewOkType>
	FORCEINLINE TResult<NewOkType, ErrorType> Map(TFunction<NewOkType(const OkType&)> mapper)
	{
		if (const auto& okOption = GetOk())
		{
			const auto& okValue = okOption.GetValue();
			return TResult::Error(mapper(okValue));
		}
		return *this;
	}


	~TResult() = default;

private:
	FORCEINLINE TResult(const OkType& okValue)
		: _okValue(okValue),
		  _errorValue({})
	{
	}

	FORCEINLINE TResult(OkType&& okValue) noexcept
		: _okValue(okValue),
		  _errorValue({})
	{
	}

	FORCEINLINE TResult(const ErrorType& errorValue)
		: _okValue({}),
		  _errorValue(errorValue)
	{
	}

	FORCEINLINE TResult(ErrorType&& errorValue) noexcept
		: _okValue({}),
		  _errorValue(errorValue)
	{
	}

	TOptional<OkType> _okValue;
	TOptional<ErrorType> _errorValue;
};

#define MATCH_RESULT(result, okVariableName, okBlock, errorVariableName, errorBlock) \
	do \
	{ \
		const TOptional<decltype(result)::FOkType> okOption = (result).GetOk(); \
		if ((okOption).IsSet()) \
		{ \
			const decltype(result)::FOkType& okVariableName = okOption.GetValue(); \
			{ okBlock } \
		} \
		else \
		{ \
			const TOptional<decltype(result)::FErrorType> errorOption = (result).GetError(); \
			const decltype(result)::FErrorType& errorVariableName = errorOption.GetValue(); \
			{ errorBlock } \
		} \
	} \
	while (false);
