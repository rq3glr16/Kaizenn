#pragma once
#ifndef HIDDENTEXT_H
#define HIDDENTEXT_H
class HiddenText
{
public:
	template<int TextSize>
	constexpr HiddenText(char(&Text)[TextSize])
	{
		// Copy text size into class instance
		this->m_TextSize = TextSize;

		// Copy Text Into m_Text
		for (int i = 0; i < this->m_TextSize; ++i)
		{
			this->m_Text[i] = Text[i];
		}

		// Add NULL terminator to copied string
		this->m_Text[this->m_TextSize] = '\0';

		// Erase original string
		for (int i = 0; i < this->m_TextSize; ++i)
		{
			Text[i] = '\0';
		}

		// XOR copied string with key 'x' to encrypt
		for (int i = 0; i < this->m_TextSize; ++i)
		{
			this->m_Text[i] ^= 'appl';
		}
	}

	char* GetDecryptedString(void);
	void DecryptString(void);
	~HiddenText(void);
private:
	char m_Text[256];
	int  m_TextSize;
};
#endif