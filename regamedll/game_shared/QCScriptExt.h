/*
* 
Created Date: Jul 13 2020

Modern Warfare Dev Team
	Code - Luna the Reborn

*/

#pragma once
#ifndef _QC_SCRIPT_EXT_
#define _QC_SCRIPT_EXT_

#include <list>
#include <variant>

namespace QCScript
{
	using namespace std;

	/*
	* The expression result.
	* std::variant<> is a safer version i.e. C++ version of 'union'
	*/
	using Result = variant<bool, int, float, char, Vector2D, Vector>;

	typedef enum : size_t
	{
		BOOL = 0,
		INT,
		FLOAT,
		CHAR,
		VEC2,
		VEC3,
	}
	EResultTypes;

	struct CBaseExpression
	{
		virtual ~CBaseExpression() {}

		virtual bool	Act			(void) { return false; }
		virtual Result	Evaluate	(void) const { return false; }
		virtual size_t	EvaluateType(void) const
		{
			static Result r = Evaluate();
			static auto iType = r.index();

			return iType;
		}
	};

	template<typename T>
	struct CBaseUnaryOperation : public CBaseExpression
	{
		CBaseUnaryOperation(T* ptr) :
			m_ptr(ptr),
			m_bShouldFreeRValue(false)
		{
			assert(m_ptr != nullptr);
		}

		CBaseUnaryOperation(const T value) :
			m_ptr(new T(value)),
			m_bShouldFreeRValue(true)
		{
			assert(m_ptr != nullptr);
		}

		~CBaseUnaryOperation() override
		{
			if (m_bShouldFreeRValue)
				delete m_ptr;

			m_ptr = nullptr;
		}

		T* m_ptr{ nullptr };
		bool m_bShouldFreeRValue{ false };
	};

	template<typename T>
	struct CBaseBinaryOperationConst : public CBaseExpression
	{
		CBaseBinaryOperationConst(const T* lhs, const T* rhs) :
			m_pLHS(lhs),
			m_pRHS(rhs),
			m_bShouldFreeRValue(false)
		{
			assert(m_pLHS != nullptr && m_pRHS != nullptr);
		}

		CBaseBinaryOperationConst(const T* ptr, const T value) :
			m_pRHS(new T(value)),
			m_pLHS(ptr),
			m_bShouldFreeRValue(true)
		{
			assert(m_pLHS != nullptr && m_pRHS != nullptr);
		}

		~CBaseBinaryOperationConst() override
		{
			if (m_bShouldFreeRValue)
				delete m_pRHS;

			m_pRHS = nullptr;
		}

		const T* m_pLHS{ nullptr };
		const T* m_pRHS{ nullptr };
		bool m_bShouldFreeRValue{ false };
	};

	template<typename T>
	struct CBaseBinaryOperationRHSConst : public CBaseExpression
	{
		CBaseBinaryOperationRHSConst(T* lhs, const T* rhs) :
			m_pLHS(lhs),
			m_pRHS(rhs),
			m_bShouldFreeRValue(false)
		{
			assert(m_pLHS != nullptr && m_pRHS != nullptr);
		}

		CBaseBinaryOperationRHSConst(T* ptr, const T value) :
			m_pRHS(new T(value)),
			m_pLHS(ptr),
			m_bShouldFreeRValue(true)
		{
			assert(m_pLHS != nullptr && m_pRHS != nullptr);
		}

		~CBaseBinaryOperationRHSConst() override
		{
			if (m_bShouldFreeRValue)
				delete m_pRHS;

			m_pRHS = nullptr;
		}

		T* m_pLHS{ nullptr };
		const T* m_pRHS{ nullptr };
		bool m_bShouldFreeRValue{ false };
	};

	using OperationSet = list<CBaseExpression*>;

	inline void Run(OperationSet& lst)
	{
		for (auto& op : lst)
		{
			op->Act();
		}
	}

	/*
	* Assignment operator: Direct assignment.
	* a = b;
	*/
	template<typename T>
	struct CAssignmentDirect : public CBaseBinaryOperationRHSConst<T>
	{
		bool Act(void) final
		{
			*this->m_pLHS = *this->m_pRHS;
			return true;
		}

		Result Evaluate(void) const final
		{
			return *this->m_pRHS;
		}
	};

	/*
	* Comparison operator: Equal to
	* a == b;
	*/
	template<typename T>
	struct CCompOpEqualTo : public CBaseBinaryOperationConst<T>
	{
		Result Evaluate(void) const final
		{
			return static_cast<bool>(*this->m_pLHS == *this->m_pRHS);
		}
	};

	/*
	* Comparison operator: Not equal to
	* a != b;
	*/
	template<typename T>
	struct CCompOpNotEqualTo : public CBaseBinaryOperationConst<T>
	{
		Result Evaluate(void) const final
		{
			return static_cast<bool>(*this->m_pLHS != *this->m_pRHS);
		}
	};

	/*
	* Logical operator: Logical AND
	* a && b;
	*/
};










































#endif // !_QC_SCRIPT_EXT_
