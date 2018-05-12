#pragma once
#include <string>
namespace NPCL
{
	class CKoreanIME
	{
	public:
		enum
		{
			kCusorInsert = 0,
			kCursorTyping,
		};

	public:
		CKoreanIME() = default;
		virtual ~CKoreanIME() = default;

		void InputCharacter(unsigned int input);
		bool SetCursor(int nIdx);
		int GetCursorIdx() const;

		std::string GetString() const;

		int GetStringLen() const;
		int GetState() const;
		TCHAR* CKoreanIME::GetTChar();

		void OnSpace();
		void OnDelete();

	private:
		bool __IsShift() const;
		bool __IsKorean() const;
		// 유니코드 문자값이 음절(조합)인지
		bool __IsCombinded(const TCHAR character) inline const;
		// 유니코드 문자값이 자모(단독)인지
		bool __IsSingle(const TCHAR character) inline const;

		// 조합된 문자에서 초성 추출
		int __ResolveInitialCharIdx(TCHAR character) const;
		// 조합된 문자에서 중성 추출
		int __ResolveMedivalCharIdx(TCHAR character) const;
		// 조합된 문자에서 종성 추출
		int __ResolveFinalCharidx(TCHAR character)	const;

		//입력값이 자음인지?
		bool __IsInputConsonat(unsigned int inputChar) const;

		int __InputToSingleChar(unsigned int inputChar, bool isShift) const;
		int __InputToSingleCharWithPrev(TCHAR prevchar, unsigned int inputChar) const;

		int __InputToMedivalIdx(unsigned int chararacter, bool isShift) const;
		int __InputToMedivalIdxWithPrev(const int prevcharIdx, unsigned int chararacter, bool isShift) const;

		int __InputToFinalChar(unsigned int chararacter, bool isShift) const;
		int __InputToFinalCharWithPrev(const int prevCodeIdx, unsigned int chararacter) const;

		//한글 자모코드를을 음절 초성 idx로
		int __SingleCharToInitialIdx(TCHAR firstIdx) const;
		//조합된 한글자모코드중 뒤부분을 제거
		int __SingleCharDivide(TCHAR firstIdx) const;
		// 종성idx를 초성idx
		int __ConvertFinalIdxToInitialIdx(const int nIdx) const;

		// 음절의 초성idx를을 자모 코드로
		int __InitialIdxToSingleChar(int nFirstidx) const;

		//조합된 중성중 뒷부분을 제거
		int __MedivalIdxDivide(int nIdx) const;

		// 조합된 종성idx중 뒤부분을 제거
		int __FinalIdxDivide(int nIdx) const;

		// 한글 음절 조합하기 
		TCHAR GetCombinedKorean(int firstIdx, int middleIdx, int lastIdx);

		void __MemMoveBack(int nCursorIdx);
		void __MemMoveFront(int nCursorIdx);

		void __PushChar(unsigned int character, const bool bShift);
		void __InsertCharKR(unsigned int character, const bool bShift, bool bPullPrev = false);
		void __InsertChar(unsigned int character, const bool bShift);
		void __DeleteChar();
		void __OnSpace();


		int __CalcCharSize() const;

	private:
		int nCursorState = kCusorInsert;
		int m_nCursorIdx = 0;
		int nCharlen = 0;
		TCHAR inputChar[512] = { 0, };
	};

}