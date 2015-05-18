#pragma once

#include "evmjit/DataTypes.h"

namespace dev
{
namespace evmjit
{

struct RuntimeData
{
	enum Index
	{
		Gas,
		GasPrice,
		CallData,
		CallDataSize,
		Address,
		Caller,
		Origin,
		CallValue,
		CoinBase,
		Difficulty,
		GasLimit,
		Number,
		Timestamp,
		Code,
		CodeSize,

		SuicideDestAddress = Address,		///< Suicide balance destination address
		ReturnData 		   = CallData,		///< Return data pointer (set only in case of RETURN)
		ReturnDataSize 	   = CallDataSize,	///< Return data size (set only in case of RETURN)
	};

	int64_t 	gas = 0;
	int64_t 	gasPrice = 0;
	byte const* callData = nullptr;
	uint64_t 	callDataSize = 0;
	i256 		address;
	i256 		caller;
	i256 		origin;
	i256 		callValue;
	i256 		coinBase;
	i256 		difficulty;
	i256 		gasLimit;
	uint64_t 	number = 0;
	int64_t 	timestamp = 0;
	byte const* code = nullptr;
	uint64_t 	codeSize = 0;
	h256		codeHash;
};

/// VM Environment (ExtVM) opaque type
struct Env;

enum class ReturnCode
{
	// Success codes
	Stop    = 0,
	Return  = 1,
	Suicide = 2,

	// Standard error codes
	OutOfGas           = -1,
	StackUnderflow     = -2,
	BadJumpDestination = -3,
	BadInstruction     = -4,
	Rejected           = -5, ///< Input data (code, gas, block info, etc.) does not meet JIT requirement and execution request has been rejected

	// Internal error codes
	LLVMConfigError    = -101,
	LLVMCompileError   = -102,
	LLVMLinkError      = -103,

	UnexpectedException = -111,

	LinkerWorkaround = -299,
};

class ExecutionContext
{
public:
	ExecutionContext() = default;
	ExecutionContext(RuntimeData& _data, Env* _env) { init(_data, _env); }
	ExecutionContext(ExecutionContext const&) = delete;
	ExecutionContext& operator=(ExecutionContext const&) = delete;
	EXPORT ~ExecutionContext();

	void init(RuntimeData& _data, Env* _env) { m_data = &_data; m_env = _env; }

	byte const* code() const { return m_data->code; }
	uint64_t codeSize() const { return m_data->codeSize; }
	h256 const& codeHash() const { return m_data->codeHash; }

	bytes_ref getReturnData() const;

private:
	RuntimeData* m_data = nullptr;	///< Pointer to data. Expected by compiled contract.
	Env* m_env = nullptr;			///< Pointer to environment proxy. Expected by compiled contract.
	byte* m_memData = nullptr;
	uint64_t m_memSize = 0;
	uint64_t m_memCap = 0;

public:
	/// Reference to returned data (RETURN opcode used)
	bytes_ref returnData;
};

}
}