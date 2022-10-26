#include "HiddenText.h"

HiddenText::~HiddenText(void)
{

}

// Returns the decrypted string
char* HiddenText::GetDecryptedString(void)
{
	return this->m_Text;
}

void HiddenText::DecryptString(void)
{
	// XOR copied string with key 'x' to decrypt
	for (int i = 0; i < this->m_TextSize; ++i)
	{
		this->m_Text[i] ^= 'appl';
	}

	return;
}