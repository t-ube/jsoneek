#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <new>

#define  JSON_TRUE   "true"
#define  JSON_FALSE  "false"
#define  JSON_NULL   "null"
#define  PATHKEY_SEP  '/'
#define  TEXT_BUFFER  1024

enum JSON_CRTYPE {
	CRT_NONE = 0, // none
	CRT_CR,       // \n
	CRT_CRLF,     // \r\n
};

// Type of Value
enum JSON_VALTYPE {
	VAL_UNKNOWN = 0, // unknown
	VAL_NULL,        // null
	VAL_BOOL,        // boolean
	VAL_NUMBER,      // Number
	VAL_TEXT,        // Text
	VAL_OBJECT,      // Object
	VAL_ARRAY,       // Array
};

class jsoneek
{
public:
	// Type of return
	

public:
	jsoneek(){
		m_IsObject = false;
		m_HasArrayValue = false;
		m_pParentNode = NULL;
	};
	jsoneek(std::string Key){ 
		m_IsObject = false;
		m_HasArrayValue = false;
		m_Key = Key;
		m_pParentNode = NULL;
	};
	jsoneek(std::string Key, std::string Value){
		this->jsoneek::jsoneek(Key);
		this->SetText(Value);
	};
	jsoneek(std::string Key, int Value) {
		this->jsoneek::jsoneek(Key);
		this->SetInt(Value);
	};
	jsoneek(std::string Key, double Value) {
		this->jsoneek::jsoneek(Key);
		this->SetDouble(Value);
	};
	jsoneek(std::string Key, bool Value) {
		this->jsoneek::jsoneek(Key);
		this->SetBool(Value);
	};
	~jsoneek(){ Delete(); };

public:
	typedef struct _ANALIZE_INFO{
		
		std::ifstream*	pFile;						// file stream
		char			MemReadText[TEXT_BUFFER];	// Text of memory / Max:1024
		unsigned int	MemAnalizePos;				// Analize position
		unsigned long	MemReadCount;				// Read length
		unsigned long	TotalAnalizeCount;			// Charactor length
		bool			IsWQuote;					// Count of "
		bool			ValueAnalizing;				// Flag of analizing value
		std::string		NextKey;					// Next key name
		std::string		StockText;					// Buffer of charactor
		jsoneek*		pNode;						// Current node

	}ANALIZE_INFO, *PANALIZE_INFO;

protected:
	bool                                      m_IsObject;
	bool                                      m_HasArrayValue;
	std::string                               m_Key;
	std::string                               m_Value;
	jsoneek*                                  m_pParentNode;
	std::vector<jsoneek*>                     m_ChildNodes;
	std::multimap<std::string, jsoneek*>      m_ChildNodeIndex;
	std::pair<unsigned long, unsigned long>   m_BetweenPos;

public:
	static jsoneek* AnalizeStream(std::ifstream* pFile)
	{
		ANALIZE_INFO  Info;

		Info.IsWQuote = false;
		Info.pFile = pFile;
		Info.pNode = NULL;
		Info.TotalAnalizeCount = 0;
		Info.MemAnalizePos = 0;
		Info.MemReadCount = 0;
		Info.ValueAnalizing = false;
		memset(Info.MemReadText, 0x00, TEXT_BUFFER);

		Analize(&Info);

		return (Info.pNode);
	}

	static jsoneek* AnalizeText(std::string text)
	{
		ANALIZE_INFO  Info;

		Info.IsWQuote = false;
		Info.pFile = NULL;
		Info.pNode = NULL;
		Info.TotalAnalizeCount = 0;
		Info.MemAnalizePos = 0;
		Info.MemReadCount = 0;
		Info.ValueAnalizing = false;
		memset(Info.MemReadText, 0x00, TEXT_BUFFER);

		if (strlen(text.c_str()) < TEXT_BUFFER) {
			strcpy_s(Info.MemReadText, text.c_str());
			Info.MemReadCount = strlen(Info.MemReadText);
			Analize(&Info);
		}

		return (Info.pNode);
	}

	void SetHasArrayValue(void) { m_HasArrayValue = true; };
	void SetIsObject(void) { m_IsObject = true; };
	void SetValue(std::string Value) { m_Value = Value; };
	void SetText(std::string Value) { m_Value = "\"" + Value + "\""; };
	void SetDouble(double Value) { m_Value = std::to_string(Value); };
	void SetInt(int Value) { m_Value = std::to_string(Value); };
	void SetBool(bool Value) { m_Value = Value ? JSON_TRUE : JSON_FALSE; };
	void SetNull(void) { m_Value = JSON_NULL; };

	static std::string RemoveWQuote(std::string Text)
	{
		std::string  NewText;

		if (Text.length() > 0)
		{
			if ((Text.at(0) == '\"') &&
				(Text.at(Text.length() - 1) == '\"'))
			{
				Text.erase(Text.length() - 1, 1);
				Text.erase(0, 1);
			}
		}

		NewText = Text;

		return (NewText);
	}

	static std::string Replace(std::string Text, std::string Search, std::string Replace)
	{
		std::string::size_type pos = Text.find(Search);
		while (pos != std::string::npos) {
			Text.replace(pos, Search.size(), Replace);
			pos = Text.find(Search, pos + Replace.size());
		}

		return Text;
	}

	std::string GetJsonText(JSON_CRTYPE CRType, bool CRAddSpace) const
	{
		std::string  KV = "";
		std::string  CR = "";
		std::string  Space = "";

		if (this) {
			if (CRAddSpace) {
				for (int Loop = 0, Count = this->GetNestCount(); Loop < Count; Loop++) {
					Space += "    ";
				}
			}

			switch (CRType)
			{
			case CRT_CR:
				CR = "\n";
				CR += Space;
				break;
			case CRT_CRLF:
				CR = "\r\n";
				CR += Space;
				break;
			case CRT_NONE:
			default:
				break;
			}

			if (!this->GetKey().empty()) {
				KV = "\"" + this->GetKey() + "\"";
				KV += ": ";
			}

			if (this->IsObject()) {
				KV += "{";
				KV += CR;

				int Count = this->m_ChildNodes.size();

				for (int Loop = 0; Loop < Count; Loop++) {
					if (Loop != 0) {
						KV += ",";
						KV += CR;
					}
					KV += this->m_ChildNodes[Loop]->GetJsonText(CRType, CRAddSpace);
				}

				KV += CR;
				KV += "}";
			}
			else if (this->HasArrayValue()) {
				KV += "[";

				int Count = this->m_ChildNodes.size();

				for (int Loop = 0; Loop < Count; Loop++) {
					if (Loop != 0) {
						KV += ",";
					}
					KV += this->m_ChildNodes[Loop]->GetJsonText(CRType, CRAddSpace);
				}

				KV += "]";
			} else {
				KV += this->GetValue();
			}
		}

		return KV;
	}

	static JSON_VALTYPE  GetValueType(std::string Value)
	{
		JSON_VALTYPE Type = VAL_UNKNOWN;

		if (Value.compare(JSON_NULL) == 0) {
			Type = VAL_NULL;
		} else if (Value.find(JSON_TRUE) != -1) {
			Type = VAL_BOOL;
		} else if (Value.find(JSON_FALSE) != -1) {
			Type = VAL_BOOL;
		} else if (Value.length() > 0) {
			if ((Value.at(0) == '\"') &&
				(Value.at(Value.length() - 1) == '\"')) {
				Type = VAL_TEXT;
			} else {
				Type = VAL_NUMBER;
			}
		}

		return Type;
	}

	JSON_VALTYPE GetValueType(void) const
	{
		JSON_VALTYPE Type = VAL_UNKNOWN;

		if (this) {
			if (this->HasArrayValue()) {
				Type = VAL_ARRAY;
			}
			else if (this->IsObject()) {
				Type = VAL_OBJECT;
			}
			else {
				Type = jsoneek::GetValueType(this->m_Value);
			}
		}

		return Type;
	}

	std::string GetValue(void) const
	{
		std::string Value = "";

		if (this) {
			if (this->HasArrayValue()) {
				int Count = this->m_ChildNodes.size();

				for (int Loop = 0; Loop < Count; Loop++) {
					if (Loop != 0) {
						Value.push_back(',');
					}
					Value += this->m_ChildNodes[Loop]->GetValue();
				}
			} else {
				Value = m_Value;
			}
		}

		return Value;
	}

	std::string GetTextValue(void) const
	{
		std::string Value = "";

		if (this) {
			Value = jsoneek::RemoveWQuote(GetValue());
		}

		return Value;
	}

	bool GetBoolValue(void) const
	{
		return GetValue().find(JSON_TRUE) != -1 ? true : false;
	}

	int GetIntValue(void) const
	{
		return _atoi_l(GetValue().c_str(), NULL);
	}

	double GetDoubleValue(void) const
	{
		return _atof_l(GetValue().c_str(), NULL);
	}

	jsoneek* GetRoot(void)
	{
		jsoneek  *pNode = this;

		while (!pNode->IsRoot()) {
			pNode = pNode->m_pParentNode;
		}

		return pNode;
	}

	jsoneek* GetParent(void)
	{
		return this->m_pParentNode;
	}

	jsoneek* GetChildNode(std::string Key, uint32_t Index = 0)
	{
		jsoneek*			pNode = NULL;
		std::vector<jsoneek*>	List;

		if (this) {
			this->GetChildNodes(Key, List);

			if ((Index >= 0) && (Index < List.size())) {
				pNode = List[Index];
			}
		}

		return (pNode);
	}

	void GetChildNodes(std::vector<jsoneek*> &List)
	{
		int Count = this->m_ChildNodes.size();

		if (Count) {
			for (int Loop = 0; Loop < Count; Loop++) {
				List.push_back(this->m_ChildNodes[Loop]);
			}
		}
		return;
	}

	void DeleteChildNode(std::string Key) {
		jsoneek*			pNode = NULL;
		std::vector<jsoneek*>	List;

		if (this)
		{
			auto itr = this->m_ChildNodeIndex.lower_bound(Key);
			auto last = this->m_ChildNodeIndex.upper_bound(Key);

			while (itr != last) {
				auto itr2 = this->m_ChildNodes.begin();
				auto last2 = this->m_ChildNodes.end();

				while (itr2 != last2) {
					if (itr->second == *itr2) {
						this->m_ChildNodes.erase(itr2);
						itr2 = this->m_ChildNodes.begin();
						last2 = this->m_ChildNodes.end();
					} else {
						++itr2;
					}
				}

				itr->second->Delete();
				++itr;
			}

			this->m_ChildNodeIndex.erase(Key);
		}

		return;
	}

	jsoneek* GetChildNode(std::string Key, std::string Value)
	{
		jsoneek*			pNode = NULL;
		std::vector<jsoneek*>	List;

		if (this) {
			this->GetChildNodes(Key, List);

			for (int Loop = 0, Count = (int)List.size(); Loop < Count; Loop++) {
				if (this->m_ChildNodes[Loop]->m_Value == Value) {
					pNode = this->m_ChildNodes[Loop];
					break;
				}
			}
		}

		return (pNode);
	}

	void GetChildNodes(std::string Key, std::vector<jsoneek*> &List)
	{
		std::stringstream   ss{ Key };
		std::string         buf;
		jsoneek*            pNode = NULL;
		std::vector<jsoneek*>  ListTemp, ResultList;

		ListTemp.push_back(this);

		while (std::getline(ss, buf, PATHKEY_SEP)) {
			for (uint32_t Loop = 0; Loop < ListTemp.size(); Loop++) {
				jsoneek::GetChildNodes(ListTemp[Loop], buf, ResultList);
			}

			if (ResultList.size() <= 0) {
				List.erase(List.begin(), List.end());
				break;
			} else {
				ListTemp.resize(ResultList.size());
				std::copy(ResultList.begin(), ResultList.end(), ListTemp.begin());

				List.resize(ResultList.size());
				std::copy(ResultList.begin(), ResultList.end(), List.begin());

				ResultList.erase(ResultList.begin(), ResultList.end());
			}
		}

		return;
	}

	jsoneek* SearchNode(std::string Key, uint32_t Index = 0)
	{
		jsoneek*			pNode = NULL;
		std::vector<jsoneek*>	List;

		if (this) {
			this->SearchNodes(Key, List);

			if ((Index >= 0) && (Index < List.size())) {
				pNode = List[Index];
			}
		}

		return (pNode);
	}

	void SearchNodes(std::string Key, std::vector<jsoneek*> &List)
	{
		std::stringstream      ss{ Key };
		std::string            buf;
		jsoneek*               pNode = NULL;
		std::vector<jsoneek*>  ListTemp, ResultList;

		pNode = this->GetRoot();

		if (std::getline(ss, buf, PATHKEY_SEP)) {
			if (pNode->GetKey() == buf) {
				ListTemp.push_back(pNode);

				while (std::getline(ss, buf, PATHKEY_SEP)) {
					for (uint32_t Loop = 0; Loop < ListTemp.size(); Loop++) {
						jsoneek::GetChildNodes(ListTemp[Loop], buf, ResultList);
					}

					if (ResultList.size() <= 0) {
						List.erase(List.begin(), List.end());
						break;
					} else {
						ListTemp.resize(ResultList.size());
						std::copy(ResultList.begin(), ResultList.end(), ListTemp.begin());

						List.resize(ResultList.size());
						std::copy(ResultList.begin(), ResultList.end(), List.begin());

						ResultList.erase(ResultList.begin(), ResultList.end());
					}
				}
			}
		}

		return;
	}

	int GetNestCount(void) const
	{
		int Count = 0;
		const jsoneek  *pNode = this;

		while (!pNode->IsRoot()) {
			pNode = pNode->m_pParentNode;
			Count += 1;
		}

		return  Count;
	}

	std::string GetPathKey(void) const
	{
		std::string Path = jsoneek::GetFullPathKey(this->m_pParentNode);
		return Path;
	}

	std::string GetFullPathKey(void) const
	{
		std::string Path = jsoneek::GetFullPathKey(this);

		return Path;
	}

	jsoneek* AddNode(std::string Key)
	{
		std::stringstream   ss{ Key };
		std::string         buf, next;
		jsoneek*          pNode = NULL;

		if (std::getline(ss, buf, PATHKEY_SEP)) {
			if (this->GetValue() == buf) {
				if (std::getline(ss, buf, PATHKEY_SEP)) {
					next += buf;

					while (std::getline(ss, buf, PATHKEY_SEP)) {
						next += PATHKEY_SEP;
						next += buf;
					}

					pNode = AddNode(this->GetRoot(), next);
				}
			}
		}

		return pNode;
	}

	void AddNoneKeyNode(jsoneek* pNode){
		if (pNode) {
			pNode->m_pParentNode = this;
			m_ChildNodes.push_back(pNode);
			m_ChildNodeIndex.insert(std::make_pair("", pNode));
		}
	};

	void AddNode(jsoneek* pNode){
		if (pNode) {
			if (pNode->m_Key.length() > 0) {
				m_IsObject = true;
			}
			pNode->m_pParentNode = this;
			m_ChildNodes.push_back(pNode);
			m_ChildNodeIndex.insert(std::make_pair(pNode->m_Key, pNode));
		}
	};

	void AddText(std::string Key, std::string Value, bool Create = true) {
		jsoneek* pNode = GetChildNode(Key);
		if (pNode == NULL) {
			if (Create) {
				jsoneek* pNew = new jsoneek(Key, Value);
				if (pNew) {
					if (pNew->m_Key.length() > 0) {
						m_IsObject = true;
					}
					pNew->m_pParentNode = this;
					m_ChildNodes.push_back(pNew);
					m_ChildNodeIndex.insert(std::make_pair(pNew->m_Key, pNew));
				}
			}
		}
		else {
			m_IsObject = true;
			pNode->SetText(Value);
		}
	};

	void AddBool(std::string Key, bool Value, bool Create = true) {
		jsoneek* pNode = GetChildNode(Key);
		if (pNode == NULL) {
			if (Create) {
				jsoneek* pNew = new jsoneek(Key, Value);
				if (pNew) {
					if (pNew->m_Key.length() > 0) {
						m_IsObject = true;
					}
					pNew->m_pParentNode = this;
					m_ChildNodes.push_back(pNew);
					m_ChildNodeIndex.insert(std::make_pair(pNew->m_Key, pNew));
				}
			}
		}
		else {
			m_IsObject = true;
			pNode->SetBool(Value);
		}
	};

	void AddNull(std::string Key, bool Create = true) {
		jsoneek* pNode = GetChildNode(Key);
		if (pNode == NULL) {
			if (Create) {
				jsoneek* pNew = new jsoneek(Key);
				if (pNew) {
					if (pNew->m_Key.length() > 0) {
						m_IsObject = true;
					}
					pNew->SetNull();
					pNew->m_pParentNode = this;
					m_ChildNodes.push_back(pNew);
					m_ChildNodeIndex.insert(std::make_pair(pNew->m_Key, pNew));
				}
			}
		}
		else {
			m_IsObject = true;
			pNode->SetNull();
		}
	};

	void AddDouble(std::string Key, double Value, bool Create = true) {
		jsoneek* pNode = GetChildNode(Key);
		if (pNode == NULL) {
			if (Create) {
				jsoneek* pNew = new jsoneek(Key, Value);
				if (pNew) {
					if (pNew->m_Key.length() > 0) {
						m_IsObject = true;
					}
					pNew->m_pParentNode = this;
					m_ChildNodes.push_back(pNew);
					m_ChildNodeIndex.insert(std::make_pair(pNew->m_Key, pNew));
				}
			}
		}
		else {
			m_IsObject = true;
			pNode->SetDouble(Value);
		}
	};

	void AddInt(std::string Key, int Value, bool Create = true) {
		jsoneek* pNode = GetChildNode(Key);
		if (pNode == NULL) {
			if (Create) {
				jsoneek* pNew = new jsoneek(Key, Value);
				if (pNew) {
					if (pNew->m_Key.length() > 0) {
						m_IsObject = true;
					}
					pNew->m_pParentNode = this;
					m_ChildNodes.push_back(pNew);
					m_ChildNodeIndex.insert(std::make_pair(pNew->m_Key, pNew));
				}
			}
		}
		else {
			m_IsObject = true;
			pNode->SetInt(Value);
		}
	};

	std::string  GetKey(void) const { return m_Key; };

	bool	IsCorrected(void) const { return (m_BetweenPos.first < m_BetweenPos.second); };
	bool	IsRoot(void) const { return (m_pParentNode == NULL); };
	bool	IsObject(void) const { return (m_IsObject); };
	bool	IsChilds(void) const { return (m_ChildNodes.size() > 0); };
	bool	HasArrayValue(void) const { return (m_HasArrayValue); };

protected:
	void Delete(void)
	{
		int Count = 0;

		if (this)
		{
			Count = this->m_ChildNodes.size();

			if (Count)
			{
				for (int Loop = 0; Loop < Count; Loop++)
				{
					delete this->m_ChildNodes[Loop];
					this->m_ChildNodes[Loop] = NULL;
				}

				std::vector<jsoneek*>::iterator begin = this->m_ChildNodes.begin();
				std::vector<jsoneek*>::iterator end = this->m_ChildNodes.end();

				this->m_ChildNodes.erase(begin, end);
			}
		}

		return;
	}

	static void Analize(ANALIZE_INFO *pInfo)
	{
		do
		{
			for (pInfo->MemAnalizePos; pInfo->MemAnalizePos < pInfo->MemReadCount; pInfo->MemAnalizePos++)
			{
				if (pInfo->MemReadText[pInfo->MemAnalizePos] == '\"')
				{
					pInfo->StockText += pInfo->MemReadText[pInfo->MemAnalizePos];
					pInfo->IsWQuote = !pInfo->IsWQuote;
					pInfo->TotalAnalizeCount += 1;
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == '{') &&
					(!pInfo->IsWQuote))
				{
					jsoneek  *pNewNode = new jsoneek(pInfo->NextKey);

					pInfo->NextKey.clear();

					pInfo->ValueAnalizing = false;

					pNewNode->m_BetweenPos.first = pInfo->TotalAnalizeCount;
					pNewNode->SetIsObject();
					pInfo->TotalAnalizeCount += 1;

					if (pInfo->pNode != NULL)
					{
						pInfo->pNode->AddNode(pNewNode);

						pInfo->pNode = pNewNode;
						pInfo->MemAnalizePos += 1;
						Analize(pInfo);
					}
					else
					{
						pInfo->pNode = pNewNode;
					}
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == '}') &&
					(!pInfo->IsWQuote))
				{
					if (pInfo->pNode != NULL)
					{
						if (pInfo->ValueAnalizing)
						{
							jsoneek  *pNewNode = new jsoneek(pInfo->NextKey);
							pInfo->NextKey.clear();

							pNewNode->SetValue(pInfo->StockText);
							pInfo->StockText.clear();
							pInfo->ValueAnalizing = false;

							pInfo->pNode->AddNode(pNewNode);
						}

						pInfo->pNode->m_BetweenPos.second = pInfo->TotalAnalizeCount;

						if (pInfo->pNode->m_pParentNode != NULL)
						{
							pInfo->pNode = pInfo->pNode->m_pParentNode;
						}
					}

					pInfo->TotalAnalizeCount += 1;
					goto End_Function;
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == ':') && (!pInfo->IsWQuote))
				{
					// ƒL[‚Ì " ‚ðÁ‹Ž‚µ‚Ü‚· //
					pInfo->NextKey = jsoneek::RemoveWQuote(pInfo->StockText);
					pInfo->StockText.clear();
					pInfo->ValueAnalizing = true;
					pInfo->TotalAnalizeCount += 1;
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == ',') && (!pInfo->IsWQuote))
				{
					if (pInfo->pNode != NULL)
					{
						jsoneek  *pNewNode = NULL;

						if (pInfo->ValueAnalizing)
						{
							if (pInfo->pNode->HasArrayValue())
							{
								pNewNode = new jsoneek();
								pNewNode->SetValue(pInfo->StockText);
								pInfo->StockText.clear();
								pInfo->pNode->AddNoneKeyNode(pNewNode);
							}
							else
							{
								pNewNode = new jsoneek(pInfo->NextKey);
								pInfo->NextKey.clear();
								pNewNode->SetValue(pInfo->StockText);
								pInfo->StockText.clear();
								pInfo->ValueAnalizing = false;

								pInfo->pNode->AddNode(pNewNode);
							}
						}
					}

					pInfo->TotalAnalizeCount += 1;
				}
				else if (((pInfo->MemReadText[pInfo->MemAnalizePos] == ' ') ||
					(pInfo->MemReadText[pInfo->MemAnalizePos] == '\r') ||
					(pInfo->MemReadText[pInfo->MemAnalizePos] == '\n'))
					&& (!pInfo->IsWQuote))
				{
					pInfo->TotalAnalizeCount += 1;
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == '[') && (!pInfo->IsWQuote))
				{
					jsoneek  *pNewNode = new jsoneek(pInfo->NextKey);
					pInfo->NextKey.clear();

					pNewNode->SetValue(pInfo->StockText);
					pInfo->StockText.clear();

					pNewNode->SetHasArrayValue();

					if (pInfo->pNode != NULL)
					{
						pInfo->pNode->AddNode(pNewNode);

						pInfo->pNode = pNewNode;
						pInfo->MemAnalizePos += 1;
						Analize(pInfo);
					}
					else
					{
						pInfo->pNode = pNewNode;
					}

					pInfo->TotalAnalizeCount += 1;
				}
				else if ((pInfo->MemReadText[pInfo->MemAnalizePos] == ']') && (!pInfo->IsWQuote))
				{
					if (pInfo->pNode != NULL)
					{
						jsoneek  *pNewNode = NULL;

						if (pInfo->ValueAnalizing)
						{
							if (pInfo->pNode->HasArrayValue())
							{
								pNewNode = new jsoneek();
								pNewNode->SetValue(pInfo->StockText);
								pInfo->StockText.clear();
								pInfo->pNode->AddNoneKeyNode(pNewNode);
							}
						}

						pInfo->ValueAnalizing = false;

						if (pInfo->pNode->m_pParentNode != NULL)
						{
							pInfo->pNode = pInfo->pNode->m_pParentNode;
						}
					}

					pInfo->TotalAnalizeCount += 1;
					goto End_Function;
				}
				else if ((pInfo->ValueAnalizing) || (pInfo->IsWQuote))
				{
					pInfo->StockText += pInfo->MemReadText[pInfo->MemAnalizePos];
					pInfo->TotalAnalizeCount += 1;
				}
				else
				{
					pInfo->TotalAnalizeCount += 1;
				}
			}

			if (pInfo->pFile)
			{
				pInfo->pFile->read(pInfo->MemReadText, 1024);
				pInfo->MemReadCount = strlen(pInfo->MemReadText);
				pInfo->MemAnalizePos = 0;
			}

		} while (pInfo->MemReadCount > 0);

	End_Function:

		return;
	}

	static void GetChildNodes(jsoneek* pNode, std::string Key, std::vector<jsoneek*> &List)
	{
		if (pNode) {
			auto itr = pNode->m_ChildNodeIndex.lower_bound(Key);
			auto last = pNode->m_ChildNodeIndex.upper_bound(Key);

			while (itr != last) {
				List.push_back(itr->second);
				++itr;
			}
		}

		return;
	}

	std::string	GetFullPathKey(const jsoneek* pNode) const
	{
		std::string  Path = "";

		while ((pNode != NULL) && (!pNode->IsRoot()))
		{
			if (Path.length())
			{
				Path = PATHKEY_SEP + Path;
			}

			Path = pNode->GetKey() + Path;
			pNode = pNode->m_pParentNode;
		};

		Path = PATHKEY_SEP + Path;

		return Path;
	}

	jsoneek*  AddNode(jsoneek* pNode, std::string Key)
	{
		std::stringstream   ss{ Key };
		std::string         buf, next;
		jsoneek*            pChiled = NULL;
		std::vector<jsoneek*>  ListTemp, ResultList;

		if (std::getline(ss, buf, PATHKEY_SEP)) {
			ResultList.erase(ResultList.begin(), ResultList.end());
			jsoneek::GetChildNodes(pNode, buf, ResultList);

			if (ResultList.size() <= 0) {
				pChiled = new jsoneek(buf);
				pNode->AddNode(pChiled);
				ResultList.push_back(pChiled);
			}

			if (std::getline(ss, buf, PATHKEY_SEP)) {
				if (pChiled != NULL)
				{
					pChiled->SetIsObject();
				}

				next += buf;

				while (std::getline(ss, buf, PATHKEY_SEP)) {
					next += PATHKEY_SEP;
					next += buf;
				}

				for (uint32_t Loop = 0; Loop < ResultList.size(); Loop++) {
					pChiled = AddNode(ResultList[Loop], next);
				}
			}
		}

		return pChiled;
	}

};

class jsoneekArray : protected jsoneek
{
public:
	jsoneekArray(std::string Key) {
		m_IsObject = false;
		m_HasArrayValue = true;
		m_Key = Key;
		m_pParentNode = NULL;
	};

	~jsoneekArray() {
		Delete();
	}

	void AddText(std::string Value) {
		this->jsoneek::AddText("", Value, true);
	};

	void AddBool(bool Value) {
		this->jsoneek::AddBool("", Value, true);
	};

	void AddNull(void) {
		this->jsoneek::AddNull("", true);
	};

	void AddDouble(double Value) {
		this->jsoneek::AddDouble("", Value, true);
	};

	void AddInt(int Value) {
		this->jsoneek::AddInt("", Value, true);
	};

	jsoneek* GetNode(void)
	{
		return ((jsoneek*)this);
	}

	void GetItemNodes(std::vector<jsoneek*> &List)
	{
		std::string         buf;
		jsoneek*            pNode = NULL;
		std::vector<jsoneek*>  ListTemp, ResultList;

		ListTemp.push_back(this);

		for (uint32_t Loop = 0; Loop < ListTemp.size(); Loop++) {
			jsoneek::GetChildNodes(ListTemp[Loop], buf, ResultList);
		}

		List.resize(ResultList.size());
		std::copy(ResultList.begin(), ResultList.end(), List.begin());

		ResultList.erase(ResultList.begin(), ResultList.end());

		return;
	}
};
