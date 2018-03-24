#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include <Windows.h>
#include "Windows.h"
#include <stdio.h>

using namespace std;


#define MEMPOOL_STATIC(cls, cnt) \
mutex              NPCMemoryPool<cls, cnt>::m_mutex; \
atomic<NPCMemoryPool<cls, cnt>*> NPCMemoryPool<cls, cnt>::m_instance = 0; \


#define MEMPOOL_OPERATOR(cls, cnt) \
void* operator new(size_t size) \
{\
	NPCMemoryPool<cls, cnt>* pInstnace = NPCMemoryPool<cls, cnt>::GetInstance();\
	if (pInstnace != nullptr)\
	{\
		return pInstnace->Allowcate(1);\
	}\
\
	return nullptr;\
}\
\
void operator delete(void* pDelete)\
{\
	NPCMemoryPool<cls, cnt>* pInstnace = NPCMemoryPool<cls, cnt>::GetInstance();\
	if (pInstnace != nullptr)\
	{\
		return pInstnace->Deallocate(pDelete, 1);\
	}\
}\


namespace NPCL
{
	template <typename T>
	class CMemBlock
	{
	public:
		CMemBlock::CMemBlock(int nBlockCnt, size_t nBlockSize)
			: m_nBlockTotal(nBlockCnt)
			, m_nSize(nBlockSize)
		{

		}

		//CMemBlock::CMemBlock(const CMemBlock<T>& base) <- not this
		CMemBlock(const CMemBlock<T>& base)
		{
			m_nBlockTotal = base.m_nBlockTotal;
			m_nBlockUsed = base.m_nBlockUsed;
			m_nSize = base.m_nSize;
			n_pHeapHandle = base.n_pHeapHandle;
			m_vecBlockUsed = base.m_vecBlockUsed;
		}

		CMemBlock::~CMemBlock()
		{
		}

		void CMemBlock::Clear()
		{
			if (n_pHeapHandle != nullptr)
			{
				free(n_pHeapHandle);
				//::HeapFree(GetProcessHeap(), 0, n_pHeapHandle);
				n_pHeapHandle = nullptr;
			}
		}

		int CMemBlock::GetSize() const
		{
			return m_nBlockUsed;
		}
		
		bool CMemBlock::IsFull()
		{
			if (m_nBlockUsed >= m_nBlockTotal)
			{
				return true;
			}

			return false;
		}

		bool CMemBlock::IsMyMemory(T* pDelete)
		{
			T* pBlock = reinterpret_cast<T*>(n_pHeapHandle);
			if (pDelete >= pBlock && pDelete < pBlock + (m_nBlockTotal))
			{
				return true;
			}

			return false;
		}

		void* CMemBlock::Allowcate(int count)
		{
			if (n_pHeapHandle == nullptr)
			{
				//n_pHeapHandle = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_nBlockTotal * m_nSize); //::malloc(m_nBlockTotal * m_nSize);
				n_pHeapHandle = ::malloc(m_nBlockTotal * m_nSize);

				if (n_pHeapHandle == nullptr)
				{
					return  NULL;
				}

				m_vecBlockUsed.resize(m_nBlockTotal, false);
			}

			for (int i = 0; i < m_vecBlockUsed.size(); ++i)
			{
				if (m_vecBlockUsed[i] == false)
				{
					m_vecBlockUsed[i] = true;
					++m_nBlockUsed;
					return reinterpret_cast<T*>(n_pHeapHandle) + i;
				}
			}

			return NULL;
		}

		bool CMemBlock::Deallocate(T* pDelete, int count)
		{
			if (IsMyMemory(pDelete))
			{
				int idx = pDelete - reinterpret_cast<T*>(n_pHeapHandle);
				if (idx >= 0 && idx < m_vecBlockUsed.size())
				{
					if (m_vecBlockUsed[idx] == true)
					{
						m_vecBlockUsed[idx] = false;
						--m_nBlockUsed;
					}
				}

				return true;
			}

			return false;
		}

		size_t	m_nSize;
		HANDLE n_pHeapHandle = nullptr;
		int m_nBlockTotal = 0;
		int m_nBlockUsed = 0;
		std::vector<bool> m_vecBlockUsed;
	};

	template <typename T, int Cnt>
	class NPCMemoryPool
	{
	public:
		static atomic<NPCMemoryPool<T, Cnt>*> m_instance;
		static mutex      m_mutex;
		static NPCMemoryPool<T, Cnt>* NPCMemoryPool::GetInstance()
		{
			NPCMemoryPool<T, Cnt>* pPointer = m_instance.load();
			if (pPointer == nullptr)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				pPointer = m_instance.load();
				if (pPointer == nullptr)
				{
					pPointer = new NPCMemoryPool<T, Cnt>(sizeof(T), Cnt);
					m_instance.store(pPointer);
				}
			}

			return m_instance;
		};



		NPCMemoryPool::NPCMemoryPool(size_t nSize, int nBlockCnt)
			: m_nBlockTotal(nBlockCnt),
			m_nSize(nSize)
		{
		};

		NPCMemoryPool::~NPCMemoryPool()
		{
			Clear();
		};

		void* NPCMemoryPool::Allowcate(int nCount)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			
			for (int i = 0; i < m_memBlocks.size(); ++i)
			{
				if (!m_memBlocks[i].IsFull())
				{
					void* p = m_memBlocks[i].Allowcate(nCount);
					return p;
				}
			}

			//블록이 없거나 모두 가득찬경우
			m_memBlocks.emplace_back(m_nBlockTotal, m_nSize);
			void* p = m_memBlocks.back().Allowcate(nCount);
			return p;
		};

		void NPCMemoryPool::Deallocate(void* pDelete, int nCount)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			T* pDeletePointer = reinterpret_cast<T*>(pDelete);
			for (int i = 0; i < m_memBlocks.size(); ++i)
			{
				if (m_memBlocks[i].IsMyMemory(pDeletePointer))
				{
					m_memBlocks[i].Deallocate(pDeletePointer, nCount);
				}
			}
		};

		void NPCMemoryPool::Clear()
		{
			for (auto&& item : m_memBlocks)
			{
				item.Clear();
			}

			m_memBlocks.clear();
		};

		int NPCMemoryPool::Size() const
		{
			int nSize = 0;
			for (int i = 0; i < m_memBlocks.size(); ++i)
			{
				nSize += m_memBlocks[i].GetSize();
			}

			return nSize;
		}

		int NPCMemoryPool::BlockSize() const
		{
			return m_memBlocks.size();
		}

	private:
		std::vector<CMemBlock<T>> m_memBlocks;
		size_t m_nSize  =0 ;
		int m_nBlockTotal = 0;

		//unsigned char * m_pPageBegin = __nullptr;
		//int nCurrentSize = 0;
		//HANDLE n_pHeapHandle;
	};
}
