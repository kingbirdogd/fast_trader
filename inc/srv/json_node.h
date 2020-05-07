#ifndef __DBP_CFG_SRV_JSON_NODE__
#define __DBP_CFG_SRV_JSON_NODE__
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
namespace dbp
{
	namespace cfg
	{
		namespace srv
		{
			class json_node
			{
			private:
				friend typename std::ostream;
			public:
				enum ValueType
				{
					NONE = 0,
					INT = 1,
					DOUBLE = 2,
					STRING = 3,
					JSON = 4,
					VECTOR = 5,
					BOOL = 6
				};
			public:
				typedef std::vector<json_node> json_nodeArray;
				typedef std::map<std::string, json_node> json_nodeMap;
			private:
				template <const size_t First, const size_t Second>
				class CMaxTwo
				{
				public:
					const static size_t value = First > Second ? First : Second;
				};
				template <const size_t First, const size_t Second, const size_t... Rest>
				class CMax
				{
				public:
					const static size_t value = CMaxTwo<First, CMax<Second, Rest...>::value>::value;
				};
				template <const size_t First, const size_t Second>
				class CMax<First, Second>
				{
				public:
					const static size_t value = CMaxTwo<First, Second>::value;
				};
			private:
				unsigned char m_ValueBuffer
							[
							 	 CMax
								 <
								 	 sizeof(long long),
									 sizeof(long double),
									 sizeof(std::string),
									 sizeof(json_nodeArray),
									 sizeof(json_nodeMap),
									 sizeof(bool)
								 >::value
							];
				ValueType m_ValueType;
			private:
				template <typename CreateType>
				static void CreateObject(CreateType* pPointer)
				{
					new (pPointer) CreateType();
				}
			private:
				static void StringCopy(std::string* pPointer, const std::string& value)
				{
					new (pPointer) std::string(value);
				}
				static void StringCopy(std::string* pPointer, std::string&& value)
				{
					new (pPointer) std::string(std::move(value));
				}
				static void StringCopy(std::string* pPointer, const char* pBuffer)
				{
					new (pPointer) std::string(pBuffer);
				}
				static void MapCopy(json_nodeMap* pPointer, const json_nodeMap& value)
				{
					new (pPointer) json_nodeMap(value);
				}
				static void MapCopy(json_nodeMap* pPointer, json_nodeMap&& value)
				{
					new (pPointer) json_nodeMap(std::move(value));
				}
				static json_node& MapInsert(json_nodeMap& Map, const char* pBuffer)
				{
					return Map.insert(
										//std::move
										(
												std::pair<std::string, json_node>
												(
														std::string(pBuffer),
														json_node(NONE)
												)
										)
									).first->second;
				}
				void Clean()
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							break;
						}
						case DOUBLE:
						{
							break;
						}
						case STRING:
						{
							std::string* pStr = (std::string*)pPointer;
							pStr->std::string::~basic_string();
							break;
						}
						case JSON:
						{
							json_nodeMap* pMap = (json_nodeMap*)pPointer;
							pMap->~json_nodeMap();
							break;
						}
						case VECTOR:
						{
							json_nodeArray* pVec = (json_nodeArray*)pPointer;
							pVec->~json_nodeArray();
							break;
						}
						case BOOL:
						{
							break;
						}
					}
					m_ValueType = NONE;
				}
				static void VecCopy(json_nodeArray* pPointer, const json_nodeArray& value)
				{
					new (pPointer) json_nodeArray(value);
				}
				static void VecCopy(json_nodeArray* pPointer, json_nodeArray&& value)
				{
					new (pPointer) json_nodeArray(std::move(value));
				}
				static std::string GetSelfString(const char* pszBuffer)
				{
					return std::string(pszBuffer);
				}
				void Create(const json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)(value.m_ValueBuffer);
					switch (value.m_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							long long& geter = *((long long*)pPointerValue);
							self = geter;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							long double& geter = *((long double*)pPointerValue);
							self = geter;
							break;
						}
						case STRING:
						{
							std::string* pSelf = (std::string*)pPointer;
							std::string& geter = *((std::string*)pPointerValue);
							StringCopy (pSelf, geter);
							break;
						}
						case JSON:
						{
							json_nodeMap* pSelf = (json_nodeMap*)pPointer;
							json_nodeMap& geter = *((json_nodeMap*)pPointerValue);
							MapCopy (pSelf, geter);
							break;
						}
						case VECTOR:
						{
							json_nodeArray* pSelf = (json_nodeArray*)pPointer;
							json_nodeArray& geter = *((json_nodeArray*)pPointerValue);
							VecCopy (pSelf, geter);
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							bool& geter = *((bool*)pPointerValue);
							self = geter;
							break;
						}
					}
				}
				void Create(json_node&& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)(value.m_ValueBuffer);
					switch (value.m_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							long long& geter = *((long long*)pPointerValue);
							self = geter;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							long double& geter = *((long double*)pPointerValue);
							self = geter;
							break;
						}
						case STRING:
						{
							std::string* pSelf = (std::string*)pPointer;
							std::string& geter = *((std::string*)pPointerValue);
							StringCopy (pSelf, std::move(geter));
							break;
						}
						case JSON:
						{
							json_nodeMap* pSelf = (json_nodeMap*)pPointer;
							json_nodeMap& geter = *((json_nodeMap*)pPointerValue);
							MapCopy (pSelf, std::move(geter));
							break;
						}
						case VECTOR:
						{
							json_nodeArray* pSelf = (json_nodeArray*)pPointer;
							json_nodeArray& geter = *((json_nodeArray*)pPointerValue);
							VecCopy (pSelf, std::move(geter));
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							bool& geter = *((bool*)pPointerValue);
							self = geter;
							break;
						}
					}
				}
				void Create(ValueType _ValueType = NONE)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self = 0.0;
							break;
						}
						case STRING:
						{
							std::string* pself = (std::string*)pPointer;
							CreateObject (pself);
							break;
						}
						case JSON:
						{
							json_nodeMap* pself = (json_nodeMap*)pPointer;
							CreateObject (pself);
							break;
						}
						case VECTOR:
						{
							json_nodeArray* pself = (json_nodeArray*)pPointer;
							CreateObject (pself);
							break;
						}
						case BOOL:
						{
							break;
						}
					}
				}
				bool fromStringInternal(const char* pszBuff, size_t& uPlace)
				{
					size_t& i = uPlace;
					while (0 != pszBuff[i])
					{
						if (' ' != pszBuff[i] &&
								'\n' != pszBuff[i] &&
								'\r' != pszBuff[i] &&
								'\t' != pszBuff[i])
						{
							break;
						}
						++i;
					}
			
					if (0 == pszBuff[i])
					{
						return true;
					}
			
			
					if ('{' == pszBuff[i])
					{
						Reset(JSON);
						while (true)
						{
							++i;
							while (0 != pszBuff[i])
							{
								if (' ' != pszBuff[i] &&
										'\n' != pszBuff[i] &&
										'\r' != pszBuff[i] &&
										'\t' != pszBuff[i])
								{
									break;
								}
								++i;
							}
			
							if ('}' != pszBuff[i])
							{
								std::string strGet;
								if ('"' == pszBuff[i] || '\'' == pszBuff[i])
								{
									char cQuate = pszBuff[i];
									++i;
									while (0 != pszBuff[i])
									{
										if (cQuate == pszBuff[i])
										{
											if ('\\' != pszBuff[i - 1])
											{
												break;
											}
											else
											{
												strGet[strGet.length() - 1] = cQuate;
											}
										}
										else
										{
											strGet += pszBuff[i];
										}
										++i;
									}
			
									if (0 == pszBuff[i])
									{
										Reset(JSON);
										return false;
									}
									++i;
								}
								else
								{
									while (0 != pszBuff[i])
									{
										if (' ' == pszBuff[i] ||
												'\n' == pszBuff[i] ||
												'\r' == pszBuff[i] ||
												'\t' == pszBuff[i] ||
												':' == pszBuff[i])
										{
											break;
										}
										strGet += pszBuff[i];
										++i;
									}
									if (0 == pszBuff[i])
									{
										Reset(JSON);
										return false;
									}
								}
								while (0 != pszBuff[i])
								{
									if (' ' != pszBuff[i] &&
											'\n' != pszBuff[i] &&
											'\r' != pszBuff[i] &&
											'\t' != pszBuff[i])
									{
										break;
									}
									++i;
								}
			
								if (':' != pszBuff[i])
								{
									Reset(JSON);
									return false;
								}
			
								++i;
								json_nodeMap& Map = getMap();
								json_node& newNode = MapInsert(Map, strGet.c_str());
								size_t uChildSize = 0;
								if (!newNode.fromStringInternal(pszBuff + i, uChildSize))
								{
									Reset(JSON);
									return false;
								}
			
								i += uChildSize;
			
								while (0 != pszBuff[i])
								{
									if (' ' != pszBuff[i] &&
											'\n' != pszBuff[i] &&
											'\r' != pszBuff[i] &&
											'\t' != pszBuff[i])
									{
										break;
									}
									++i;
								}
			
								if ('}' == pszBuff[i])
								{
									++i;
									return true;
								}
								else if (',' != pszBuff[i])
								{
									Reset(JSON);
									return false;
								}
							}
							else
							{
								++i;
								return true;
							}
						}
					}
					else if ('[' == pszBuff[i])
					{
						Reset(VECTOR);
						while(true)
						{
							++i;
							while (0 != pszBuff[i])
							{
								if (' ' != pszBuff[i] &&
										'\n' != pszBuff[i] &&
										'\r' != pszBuff[i] &&
										'\t' != pszBuff[i])
								{
									break;
								}
								++i;
							}
							if (']' == pszBuff[i])
							{
								++i;
								return true;
							}
							else
							{
								json_nodeArray& Vec = getVec();
								json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
								size_t uChildSize = 0;
								if (!newNode.fromStringInternal(pszBuff + i, uChildSize))
								{
									Reset(VECTOR);
									return false;
								}
								i += uChildSize;
			
								while (0 != pszBuff[i])
								{
									if (' ' != pszBuff[i] &&
											'\n' != pszBuff[i] &&
											'\r' != pszBuff[i] &&
											'\t' != pszBuff[i])
									{
										break;
									}
									++i;
								}
								if (']' == pszBuff[i])
								{
									++i;
									return true;
								}
								else if (',' != pszBuff[i])
								{
									Reset(VECTOR);
									return false;
								}
							}
						}
					}
					else if ('"' == pszBuff[i] || '\'' == pszBuff[i])
					{
						std::string strGet;
						Reset(STRING);
						char cQuate = pszBuff[i];
						++i;
						while (0 != pszBuff[i])
						{
							if (cQuate == pszBuff[i])
							{
								if ('\\' != pszBuff[i - 1])
								{
									break;
								}
								else
								{
									strGet[strGet.length() - 1] = cQuate;
								}
							}
							else
							{
								strGet += pszBuff[i];
							}
							++i;
						}
						if (0 == pszBuff[i])
						{
							Reset(STRING);
							return false;
						}
						++i;
						m_ValueType= STRING;
						void* pPointer = (void*)m_ValueBuffer;
						std::string* pSelf = (std::string*)pPointer;
						StringCopy (pSelf, strGet.c_str());
						return true;
					}
					else if (('0' <= pszBuff[i] && '9' >= pszBuff[i]) || '-' == pszBuff[i])
					{
						Reset(NONE);
						bool bDouble = false;
						size_t uStart = i;
						if ('-' == pszBuff[i])
						{
							if ('0' > pszBuff[i + 1] || '9' < pszBuff[i + 1])
							{
								Reset(NONE);
								return false;
							}
						}
						++i;
						while ('0' <= pszBuff[i] && '9' >= pszBuff[i])
						{
							++i;
						}
						if ('.' == pszBuff[i])
						{
							if ('0' > pszBuff[i + 1] || '9' < pszBuff[i + 1])
							{
								Reset(NONE);
								return false;
							}
							++i;
							bDouble = true;
							while ('0' <= pszBuff[i] && '9' >= pszBuff[i])
							{
								++i;
							}
							if ('e' == pszBuff[i] || 'E' == pszBuff[i])
							{
								++i;
								if (('0' > pszBuff[i] || '9' < pszBuff[i]) && '-' != pszBuff[i])
								{
									Reset(NONE);
									return false;
								}
								if ('-' == pszBuff[i])
								{
									++i;
									if ('0' > pszBuff[i] || '9' < pszBuff[i])
									{
										Reset(NONE);
										return false;
									}
								}
								while ('0' <= pszBuff[i] && '9' >= pszBuff[i])
								{
									++i;
								}
							}
						}
						void* pPointer = (void*)m_ValueBuffer;
						std::string Str(pszBuff + uStart, i - uStart);
						if (bDouble)
						{
							Reset(DOUBLE);
							long double& self = *((long double*)pPointer);
							try
							{
								self = std::stold(Str);
							}
							catch (...)
							{
								return false;
							}
						}
						else
						{
							Reset(INT);
							long long& self = *((long long*)pPointer);
							try
							{
								self = std::stoll(Str);
							}
							catch (...)
							{
								return false;
							}
						}
						return true;
					}
					else if ('t' == pszBuff[i])
					{
						if ('r' == pszBuff[i + 1])
						{
							if ('u' == pszBuff[i + 2])
							{
								if ('e' == pszBuff[i + 3])
								{
									Reset(BOOL);
									void* pPointer = (void*)m_ValueBuffer;
									bool& self = *((bool*)pPointer);
									self = true;
									i += 4;
									return true;
								}
							}
						}
						Reset(NONE);
						return false;
					}
					else if ('f' == pszBuff[i])
					{
						if ('a' == pszBuff[i + 1])
						{
							if ('l' == pszBuff[i + 2])
							{
								if ('s' == pszBuff[i + 3])
								{
									if ('e' == pszBuff[i + 4])
									{
										Reset(BOOL);
										void* pPointer = (void*)m_ValueBuffer;
										bool& self = *((bool*)pPointer);
										self = false;
										i += 5;
										return true;
									}
								}
							}
						}
						Reset(NONE);
						return false;
					}
					else if ('n' == pszBuff[i])
					{
						if ('u' == pszBuff[i + 1])
						{
							if ('l' == pszBuff[i + 2])
							{
								if ('l' == pszBuff[i + 3])
								{
									Reset(NONE);
									i += 4;
									return true;
								}
							}
						}
						Reset(NONE);
						return false;
					}
					else
					{
						Reset(NONE);
						return false;
					}
					return true;
				}
				json_nodeMap& getMap() const
				{
					json_nodeMap* pRt = 0;
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							break;
						}
						case DOUBLE:
						{
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							pRt = (json_nodeMap*)pPointer;
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							break;
						}
					}
					return *pRt;
				}
				json_nodeArray& getVec() const
				{
					json_nodeArray* pRt = 0;
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							break;
						}
						case INT:
						{
							break;
						}
						case DOUBLE:
						{
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							pRt = (json_nodeArray*)pPointer;
							break;
						}
						case BOOL:
						{
							break;
						}
					}
					return *pRt;
				}
				void Reset(ValueType _ValueType = NONE)
				{
					Clean();
					memset(m_ValueBuffer, 0, sizeof(m_ValueBuffer));
					m_ValueType = _ValueType;
					Create(m_ValueType);
				}
			public:
				json_node(ValueType _ValueType = NONE) :
					m_ValueBuffer{0},
					m_ValueType(_ValueType)
				{
					Create(_ValueType);
				}
				json_node(const json_node& value) :
					m_ValueBuffer{0},
					m_ValueType(value.m_ValueType)
				{
					Create(value);
				}
				json_node(json_node&& value) :
					m_ValueBuffer{0},
					m_ValueType(value.m_ValueType)
				{
					Create(std::move(value));
				}
				json_node(long long value) :
					m_ValueBuffer{0},
					m_ValueType(INT)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = value;
				}
				json_node(long value) :
					m_ValueBuffer{0},
					m_ValueType(INT)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
				}
				json_node(int value) :
					m_ValueBuffer{0},
					m_ValueType(INT)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
				}
				json_node(short value) :
					m_ValueBuffer{0},
					m_ValueType(INT)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
				}
				json_node(char value) :
					m_ValueBuffer{0},
					m_ValueType(INT)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = (long long)value;
				}
				json_node(bool value) :
					m_ValueBuffer{0},
					m_ValueType(BOOL)
				{
					void* pPointer = (void*)m_ValueBuffer;
					bool& self = *((bool*)pPointer);
					self = (bool)value;
				}
				json_node(double value) :
					m_ValueBuffer{0},
					m_ValueType(DOUBLE)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = (long double)value;
				}
				json_node(float value) :
					m_ValueBuffer{0},
					m_ValueType(DOUBLE)
				{
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = (long double)value;
				}
				json_node(const char* value) :
					m_ValueBuffer{0},
					m_ValueType(STRING)
				{
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, value);
				}
				json_node(const std::string& value) :
					m_ValueBuffer{0},
					m_ValueType(STRING)
				{
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, value);
				}
				json_node(const json_nodeMap& value) :
					m_ValueBuffer{0},
					m_ValueType(JSON)
				{
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap* pSelf = (json_nodeMap*)pPointer;
					MapCopy (pSelf, value);
				}
				json_node(const json_nodeArray& value) :
					m_ValueBuffer{0},
					m_ValueType(VECTOR)
				{
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray* pSelf = (json_nodeArray*)pPointer;
					VecCopy (pSelf, value);
				}
				json_node(std::string&& value) :
					m_ValueBuffer{0},
					m_ValueType(STRING)
				{
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, std::move(value));
				}
				json_node(json_nodeMap&& value) :
					m_ValueBuffer{0},
					m_ValueType(JSON)
				{
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap* pSelf = (json_nodeMap*)pPointer;
					MapCopy (pSelf, std::move(value));
				}
				json_node(json_nodeArray&& value) :
					m_ValueBuffer{0},
					m_ValueType(VECTOR)
				{
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray* pSelf = (json_nodeArray*)pPointer;
					VecCopy (pSelf, std::move(value));
				}
				~json_node()
				{
					Clean();
				}
				json_node& operator = (const json_node& value)
				{
					if (this == &value)
					{
						return *this;
					}
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					Create(value);
					m_ValueType = value.m_ValueType;
					return *this;
				}
				json_node& operator = (json_node&& value)
				{
					if (this == &value)
					{
						return *this;
					}
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					Create(std::move(value));
					m_ValueType = value.m_ValueType;
					return *this;
				}
				json_node& operator = (long long value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = value;
					m_ValueType = INT;
					return *this;
				}
				json_node& operator = (unsigned long long value)
				{
					return operator =((long long)value);
				}
				json_node& operator = (long value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
					m_ValueType = INT;
					return *this;
				}
				json_node& operator = (unsigned long value)
				{
					return operator =((long)value);
				}
				json_node& operator = (int value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
					m_ValueType = INT;
					return *this;
				}
				json_node& operator = (unsigned int value)
				{
					return operator =((int)value);
				}
				json_node& operator = (short value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
					m_ValueType = INT;
					return *this;
				}
				json_node& operator = (unsigned short value)
				{
					return operator =((short)value);
				}
				json_node& operator = (char value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long long& self = *((long long*)pPointer);
					self = (long long)value;
					m_ValueType = INT;
					return *this;
				}
				json_node& operator = (unsigned char value)
				{
					return operator =((char)value);
				}
				json_node& operator = (bool value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					bool& self = *((bool*)pPointer);
					self = (bool)value;
					m_ValueType = BOOL;
					return *this;
				}
				json_node& operator = (long double value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = value;
					m_ValueType = DOUBLE;
					return *this;
				}
				json_node& operator = (double value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = (long double)value;
					m_ValueType = DOUBLE;
					return *this;
				}
				json_node& operator = (float value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					long double& self = *((long double*)pPointer);
					self = (long double)value;
					m_ValueType = DOUBLE;
					return *this;
				}
				json_node& operator = (const char* value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, value);
					m_ValueType = STRING;
					return *this;
				}
				json_node& operator = (const std::string& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, value);
					m_ValueType = STRING;
					return *this;
				}
				json_node& operator = (const json_nodeMap& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap* pSelf = (json_nodeMap*)pPointer;
					MapCopy (pSelf, value);
					m_ValueType = JSON;
					return *this;
				}
				json_node& operator = (const json_nodeArray& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray* pSelf = (json_nodeArray*)pPointer;
					VecCopy (pSelf, value);
					m_ValueType = VECTOR;
					return *this;
				}
				json_node& operator = (std::string&& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					std::string* pSelf = (std::string*)pPointer;
					StringCopy (pSelf, std::move(value));
					m_ValueType = STRING;
					return *this;
				}
				json_node& operator = (json_nodeMap&& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap* pSelf = (json_nodeMap*)pPointer;
					MapCopy (pSelf, std::move(value));
					m_ValueType = JSON;
					return *this;
				}
				json_node& operator = (json_nodeArray&& value)
				{
					Clean();
					memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray* pSelf = (json_nodeArray*)pPointer;
					VecCopy (pSelf, std::move(value));
					m_ValueType = VECTOR;
					return *this;
				}
				json_node* getArrayNode (size_t _uIndex)
				{
					if (VECTOR != m_ValueType)
					{
						return 0;
					}
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray& self = *((json_nodeArray*)pPointer);
					if (self.size() <= _uIndex)
					{
						return 0;
					}
					return &(self[_uIndex]);
				}
				json_node& getArrayTop ()
				{
					if (VECTOR != m_ValueType)
					{
						return (*this)[0];
					}
					else
					{
						void* pPointer = (void*)m_ValueBuffer;
						json_nodeArray& self = *((json_nodeArray*)pPointer);
						return (*this)[self.size()];
					}
				}
				size_t getArraySize()
				{
					if (VECTOR != m_ValueType)
					{
						return 0;
					}
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray& self = *((json_nodeArray*)pPointer);
					return self.size();
				}
				json_node* getArrayNode (int value)
				{
					return this->getArrayNode((size_t)value);
				}
				json_node* getMapNode (const std::string& value)
				{
					if (JSON != m_ValueType)
					{
						return 0;
					}
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap& self = *((json_nodeMap*)pPointer);
					typename json_nodeMap::iterator it = self.find(value);
					if (self.end() == it)
					{
						return 0;
					}
					return &(it->second);
				}
				json_node* getMapNode (const char* value)
				{
					return this->getMapNode(GetSelfString(value));
				}
				void getMapKeys (std::vector<std::string>& _Vec)
				{
					_Vec.resize(0);
					if (JSON != m_ValueType)
					{
						return;
					}
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap& self = *((json_nodeMap*)pPointer);
					for (typename json_nodeMap::iterator it = self.begin();
								it != self.end(); ++it)
					{
						_Vec.push_back(it->first.c_str());
					}
				}
				json_node& operator [] (size_t _uIndex)
				{
					if (VECTOR != m_ValueType)
					{
						Clean();
						m_ValueType = VECTOR;
						Create(m_ValueType);
					}
			
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeArray& self = *((json_nodeArray*)pPointer);
					if (self.size() <= _uIndex)
					{
						self.resize(_uIndex + 1);
					}
			
					return self[_uIndex];
				}
				json_node& operator [] (const std::string& value)
				{
					if (JSON != m_ValueType)
					{
						Clean();
						m_ValueType = JSON;
						Create(m_ValueType);
					}
			
					void* pPointer = (void*)m_ValueBuffer;
					json_nodeMap& self = *((json_nodeMap*)pPointer);
					typename json_nodeMap::iterator it = self.find(value);
					if (self.end() == it)
					{
						json_node& newNode = MapInsert(self, value.c_str());
						return newNode;
					}
			
					return it->second;
				}
				json_node& operator [] (int value)
				{
					return (*this)[(size_t)value];
				}
				json_node& operator [] (const char* value)
				{
					return (*this)[GetSelfString(value)];
				}
				operator long long ()
				{
					return getInt();
				}
				operator long ()
				{
					return (long)getInt();
				}
				operator int ()
				{
					return (int)getInt();
				}
				operator short ()
				{
					return (short)getInt();
				}
				operator char ()
				{
					return (char)getInt();
				}
				operator long double ()
				{
					return getDouble();
				}
				operator double ()
				{
					return (double)getDouble();
				}
				operator float ()
				{
					return (float)getDouble();
				}
				operator std::string ()
				{
					std::string strRt;
					getString(strRt);
					return strRt;
				}
				operator bool ()
				{
					return getBool();
				}
				json_node& operator++ ()
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							++self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							++self;
							break;
						}
						case STRING:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node operator++ (int)
				{
					json_node rt(*this);
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							++self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							++self;
							break;
						}
						case STRING:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 1;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self)
							{
								self = true;
							}
							break;
						}
					}
					return rt;
				}
				json_node& operator-- ()
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							--self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							--self;
							break;
						}
						case STRING:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node operator-- (int)
				{
					json_node rt(*this);
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							--self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							--self;
							break;
						}
						case STRING:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -1;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self)
							{
								self = false;
							}
							break;
						}
					}
					return rt;
				}
				json_node& operator+= (long long value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self += value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self += (long double)value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long long lOriValue = std::stoll(str);
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = lOriValue + value;
							}
							catch(...)
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && 0 != value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator-= (long long value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = -value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self -= value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self -= (long double)value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long long lOriValue = std::stoll(str);
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = lOriValue - value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = -value;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = -value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && 0 != value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator*= (long long value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self *= value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self *= (long double)value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long long lOriValue = std::stoll(str);
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = lOriValue * value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = 0;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && 0 == value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator/= (long long value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self /= value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self /= (long double)value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long long lOriValue = std::stoll(str);
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = lOriValue / value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = 0;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && 0 == value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator%= (long long value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self %= value;
							break;
						}
						case DOUBLE:
						{
							m_ValueType = INT;
							long long& self = *((long long*)pPointer);
							self %= (long long)value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long long lOriValue = std::stoll(str);
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = lOriValue % value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = INT;
								Create(m_ValueType);
								long long& self = *((long long*)pPointer);
								self = 0;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = INT;
							Create(m_ValueType);
							long long& self = *((long long*)pPointer);
							self = 0;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && 0 == value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator+= (long value)
				{
					return ((*this) += (long long)value);
				}
				json_node& operator+= (int value)
				{
					return ((*this) += (long long)value);
				}
				json_node& operator+= (short value)
				{
					return ((*this) += (long long)value);
				}
				json_node& operator+= (char value)
				{
					return ((*this) += (long long)value);
				}
				json_node& operator-= (long value)
				{
					return ((*this) -= (long long)value);
				}
				json_node& operator-= (int value)
				{
					return ((*this) -= (long long)value);
				}
				json_node& operator-= (short value)
				{
					return ((*this) -= (long long)value);
				}
				json_node& operator-= (char value)
				{
					return ((*this) -= (long long)value);
				}
				json_node& operator*= (long value)
				{
					return ((*this) *= (long long)value);
				}
				json_node& operator*= (int value)
				{
					return ((*this) *= (long long)value);
				}
				json_node& operator*= (short value)
				{
					return ((*this) *= (long long)value);
				}
				json_node& operator*= (char value)
				{
					return ((*this) *= (long long)value);
				}
				json_node& operator/= (long value)
				{
					return ((*this) /= (long long)value);
				}
				json_node& operator/= (int value)
				{
					return ((*this) /= (long long)value);
				}
				json_node& operator/= (short value)
				{
					return ((*this) /= (long long)value);
				}
				json_node& operator/= (char value)
				{
					return ((*this) /= (long long)value);
				}
				json_node& operator%= (long value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (int value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (short value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (char value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (float value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (double value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator%= (long double value)
				{
					return ((*this) %= (long long)value);
				}
				json_node& operator+= (long double value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = DOUBLE;
							long double& self = *((long double*)pPointer);
							self = value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self += (long long)value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self += value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long double lOriValue = std::stold(str);
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = lOriValue + value;
							}
							catch(...)
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && 0.0 != value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator-= (long double value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = DOUBLE;
							long double& self = *((long double*)pPointer);
							self = -value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self -= (long long)value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self -= value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long double lOriValue = std::stold(str);
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = lOriValue - value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = -value;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = -value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = -value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && 0.0 != value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator*= (long double value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = DOUBLE;
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self *= (long long)value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self *= value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long double lOriValue = std::stold(str);
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = lOriValue * value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = 0;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && 0.0 == value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator/= (long double value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = DOUBLE;
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							self /= (long long)value;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							self /= value;
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							try
							{
								long double lOriValue = std::stold(str);
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = lOriValue / value;
							}
							catch(...)
							{
								Clean();
								m_ValueType = DOUBLE;
								Create(m_ValueType);
								long double& self = *((long double*)pPointer);
								self = 0;
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = DOUBLE;
							Create(m_ValueType);
							long double& self = *((long double*)pPointer);
							self = 0;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && 0.0 == value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator+= (double value)
				{
					return ((*this) += (long double)value);
				}
				json_node& operator+= (float value)
				{
					return ((*this) += (long double)value);
				}
				json_node& operator-= (double value)
				{
					return ((*this) -= (long double)value);
				}
				json_node& operator-= (float value)
				{
					return ((*this) -= (long double)value);
				}
				json_node& operator*= (double value)
				{
					return ((*this) *= (long double)value);
				}
				json_node& operator*= (float value)
				{
					return ((*this) *= (long double)value);
				}
				json_node& operator/= (double value)
				{
					return ((*this) /= (long double)value);
				}
				json_node& operator/= (float value)
				{
					return ((*this) /= (long double)value);
				}
				json_node& operator+= (const char* value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case INT:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self += strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case DOUBLE:
						{
							try
							{
								long double strNumber = std::stold(value);
								long double& self = *((long double*)pPointer);
								self += strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case STRING:
						{
							std::string& self = *((std::string*)pPointer);
							self += value;
							break;
						}
						case JSON:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case VECTOR:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case BOOL:
						{
							try
							{
								long long strNumber = std::stoll(value);
								bool& self = *((bool*)pPointer);
								long long newValue = self ? 1 : 0;
								newValue += strNumber;
								m_ValueType = INT;
								long long& newself = *((long long*)pPointer);
								newself = newValue;
			
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator-= (const char* value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case INT:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self -= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case DOUBLE:
						{
							try
							{
								long double strNumber = std::stold(value);
								long double& self = *((long double*)pPointer);
								self -= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case VECTOR:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case BOOL:
						{
							try
							{
								long long strNumber = std::stoll(value);
								bool& self = *((bool*)pPointer);
								long long newValue = self ? 1 : 0;
								newValue -= strNumber;
								m_ValueType = INT;
								long long& newself = *((long long*)pPointer);
								newself = newValue;
			
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator*= (const char* value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case INT:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self *= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case DOUBLE:
						{
							try
							{
								long double strNumber = std::stold(value);
								long double& self = *((long double*)pPointer);
								self *= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case VECTOR:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case BOOL:
						{
							try
							{
								long long strNumber = std::stoll(value);
								bool& self = *((bool*)pPointer);
								long long newValue = self ? 1 : 0;
								newValue *= strNumber;
								m_ValueType = INT;
								long long& newself = *((long long*)pPointer);
								newself = newValue;
			
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator/= (const char* value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case INT:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self /= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case DOUBLE:
						{
							try
							{
								long double strNumber = std::stold(value);
								long double& self = *((long double*)pPointer);
								self /= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case VECTOR:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case BOOL:
						{
							try
							{
								long long strNumber = std::stoll(value);
								bool& self = *((bool*)pPointer);
								long long newValue = self ? 1 : 0;
								newValue /= strNumber;
								m_ValueType = INT;
								long long& newself = *((long long*)pPointer);
								newself = newValue;
			
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator%= (const char* value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case INT:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self %= strNumber;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case DOUBLE:
						{
							try
							{
								long long strNumber = std::stoll(value);
								long long& self = *((long long*)pPointer);
								self %= strNumber;
								m_ValueType = INT;
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
						case STRING:
						{
							break;
						}
						case JSON:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case VECTOR:
						{
							Clean();
							memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
							m_ValueType = STRING;
							std::string* pSelf = (std::string*)pPointer;
							StringCopy (pSelf, value);
							break;
						}
						case BOOL:
						{
							try
							{
								long long strNumber = std::stoll(value);
								bool& self = *((bool*)pPointer);
								long long newValue = self ? 1 : 0;
								newValue %= strNumber;
								m_ValueType = INT;
								long long& newself = *((long long*)pPointer);
								newself = newValue;
			
							}
							catch (...)
							{
								Clean();
								memset (m_ValueBuffer, 0 ,sizeof(m_ValueBuffer));
								m_ValueType = STRING;
								std::string* pSelf = (std::string*)pPointer;
								StringCopy (pSelf, value);
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator+= (const std::string& value)
				{
					return ((*this) += value.c_str());
				}
				json_node& operator-= (const std::string& value)
				{
					return ((*this) -= value.c_str());
				}
				json_node& operator*= (const std::string& value)
				{
					return ((*this) *= value.c_str());
				}
				json_node& operator/= (const std::string& value)
				{
					return ((*this) /= value.c_str());
				}
				json_node& operator%= (const std::string& value)
				{
					return ((*this) %= value.c_str());
				}
				json_node& operator+= (bool value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = BOOL;
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							if (value)
							{
								++self;
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							if (value)
							{
								self += 1;
							}
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							if (str == "true")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								self = true;
							}
							else if (str == "false")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								if (value)
								{
									self = true;
								}
								else
								{
									self = false;
								}
							}
							else
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator-= (bool value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = BOOL;
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							if (value)
							{
								--self;
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							if (value)
							{
								self -= 1;
							}
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							if (str == "true")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								self = true;
							}
							else if (str == "false")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								if (value)
								{
									self = true;
								}
								else
								{
									self = false;
								}
							}
							else
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (!self && value)
							{
								self = true;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator*= (bool value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = BOOL;
							bool& self = *((bool*)pPointer);
							self = false;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							if (str == "true")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								if (!value)
								{
									self = false;
								}
								else
								{
									self = true;
								}
							}
							else if (str == "false")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								self = false;
							}
							else
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && !value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator/= (bool value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = BOOL;
							bool& self = *((bool*)pPointer);
							self = false;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							if (str == "true")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								if (!value)
								{
									self = false;
								}
								else
								{
									self = true;
								}
							}
							else if (str == "false")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								self = false;
							}
							else
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && !value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator%= (bool value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							m_ValueType = BOOL;
							bool& self = *((bool*)pPointer);
							self = false;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							if (!value)
							{
								self = 0;
							}
							break;
						}
						case STRING:
						{
							std::string& str = *((std::string*)pPointer);
							if (str == "true")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								if (!value)
								{
									self = false;
								}
								else
								{
									self = true;
								}
							}
							else if (str == "false")
							{
								Clean();
								m_ValueType = BOOL;
								Create(m_ValueType);
								bool& self = *((bool*)pPointer);
								self = false;
							}
							else
							{
								str += std::to_string(value);
							}
							break;
						}
						case JSON:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case VECTOR:
						{
							Clean();
							m_ValueType = BOOL;
							Create(m_ValueType);
							bool& self = *((bool*)pPointer);
							self = value;
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self && !value)
							{
								self = false;
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator+= (json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)value.m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							m_ValueType = value.m_ValueType;
							Create (value);
							break;
						}
						case INT:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self += geter;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self += (long long)geter;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self += std::stoll(geter);
									}
									catch (...)
									{
										long long lOriValue = *((long long*)pPointer);
										Clean();
										m_ValueType = STRING;
										std::string* pSelf = (std::string*)pPointer;
										StringCopy (pSelf, geter.c_str());
										(*(pSelf)) += std::to_string(lOriValue);
									}
									break;
								}
								case JSON:
								{
									long long selfValue = *((long long*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeMap& Map = getMap();
									json_node& newNode = MapInsert(Map, "");
									newNode = selfValue;
									break;
								}
								case VECTOR:
								{
									long long selfValue = *((long long*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeArray& Vec = getVec();
									json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
									newNode = selfValue;
									break;
								}
								case BOOL:
								{
									long long& self = *((long long*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (geter)
									{
										++self;
									}
									break;
								}
							}
							break;
						}
						case DOUBLE:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long double& self = *((long double*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self += (long double)geter;
									break;
								}
								case DOUBLE:
								{
									long double& self = *((long double*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self += geter;
									break;
								}
								case STRING:
								{
									long double& self = *((long double*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self += std::stold(geter);
									}
									catch (...)
									{
										long double lOriValue = *((long double*)pPointer);
										Clean();
										m_ValueType = STRING;
										std::string* pSelf = (std::string*)pPointer;
										StringCopy (pSelf, geter.c_str());
										(*(pSelf)) += std::to_string(lOriValue);
									}
									break;
								}
								case JSON:
								{
									long double selfValue = *((long double*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeMap& Map = getMap();
									json_node& newNode = MapInsert(Map, "");
									newNode = selfValue;
									break;
								}
								case VECTOR:
								{
									long double selfValue = *((long double*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeArray& Vec = getVec();
									json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
									newNode = selfValue;
									break;
								}
								case BOOL:
								{
									long double& self = *((long double*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (geter)
									{
										self += 1;
									}
									break;
								}
							}
							break;
						}
						case STRING:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									std::string& self = *((std::string*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									try
									{
										self += std::to_string(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case DOUBLE:
								{
									std::string& self = *((std::string*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									try
									{
										self += std::to_string(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case STRING:
								{
									std::string& self = *((std::string*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									self += geter;
									break;
								}
								case JSON:
								{
									std::string& selfValue = *((std::string*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeMap& Map = getMap();
									json_node& newNode = MapInsert(Map, "");
									newNode = selfValue;
									break;
								}
								case VECTOR:
								{
									std::string& selfValue = *((std::string*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeArray& Vec = getVec();
									json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
									newNode = selfValue;
									break;
								}
								case BOOL:
								{
									std::string& self = *((std::string*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									try
									{
										self += std::to_string(geter);
									}
									catch (...)
									{
									}
									break;
								}
							}
							break;
						}
						case JSON:
						{
							json_nodeMap& Map = getMap();
							json_node& newNode = MapInsert(Map, "");
							newNode = value;
							break;
						}
						case VECTOR:
						{
							json_nodeArray& Vec = getVec();
							json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
							newNode = value;
							break;
						}
						case BOOL:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									bool& self = *((bool*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									if (!self && geter != 0)
									{
										self = true;
									}
									break;
								}
								case DOUBLE:
								{
									bool& self = *((bool*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									if (!self && geter != 0.0)
									{
										self = true;
									}
									break;
								}
								case STRING:
								{
									bool& self = *((bool*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									if (!self && geter == "true")
									{
										self = true;
									}
									break;
								}
								case JSON:
								{
									long long selfValue = *((long long*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeMap& Map = getMap();
									json_node& newNode = MapInsert(Map, "");
									newNode = selfValue;
									break;
								}
								case VECTOR:
								{
									long long selfValue = *((long long*)pPointer);
									Clean();
									m_ValueType = value.m_ValueType;
									Create(value);
									json_nodeArray& Vec = getVec();
									json_node& newNode = *(Vec.insert(Vec.end(), (json_node(NONE))));
									newNode = selfValue;
									break;
								}
								case BOOL:
								{
									bool& self = *((bool*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!self && geter)
									{
										self = true;
									}
									break;
								}
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator-= (json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)value.m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							m_ValueType = value.m_ValueType;
							Create (value);
							if (INT == m_ValueType)
							{
								long long& self = *((long long*)pPointerValue);
								self = -self;
							}
							else if(DOUBLE == m_ValueType)
							{
								long double& self = *((long double*)pPointerValue);
								self = -self;
							}
							else if(BOOL == m_ValueType)
							{
								bool& self = *((bool*)pPointerValue);
								self = false;
							}
							break;
						}
						case INT:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self -= geter;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self -= (long long)geter;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self -= std::stoll(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long long& self = *((long long*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (geter)
									{
										--self;
									}
									break;
								}
							}
							break;
						}
						case DOUBLE:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long double& self = *((long double*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self -= (long double)geter;
									break;
								}
								case DOUBLE:
								{
									long double& self = *((long double*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self -= geter;
									break;
								}
								case STRING:
								{
									long double& self = *((long double*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self -= std::stold(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long double& self = *((long double*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (geter)
									{
										self -= 1;
									}
									break;
								}
							}
							break;
						}
						case STRING:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									std::string& self = *((std::string*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber - geter;
									}
									catch (...)
									{
									}
									break;
								}
								case DOUBLE:
								{
									std::string& self = *((std::string*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber - geter;
									}
									catch (...)
									{
									}
									break;
								}
								case STRING:
								{
									std::string& self = *((std::string*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										long double geterNumber = std::stold(geter);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber - geterNumber;
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									std::string& self = *((std::string*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self == "true")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										Boolself = true;
									}
									else if (self == "false")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										if (geter)
										{
											Boolself = true;
										}
										else
										{
											Boolself = false;
										}
									}
									break;
								}
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									bool& self = *((bool*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									if (!self && geter != 0)
									{
										self = true;
									}
									break;
								}
								case DOUBLE:
								{
									bool& self = *((bool*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									if (!self && geter != 0.0)
									{
										self = true;
									}
									break;
								}
								case STRING:
								{
									bool& self = *((bool*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									if (!self && geter == "true")
									{
										self = true;
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									bool& self = *((bool*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!self && geter)
									{
										self = true;
									}
									break;
								}
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator*= (json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)value.m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							m_ValueType = value.m_ValueType;
							Create (value);
							if (INT == m_ValueType)
							{
								long long& self = *((long long*)pPointerValue);
								self = 0;
							}
							else if(DOUBLE == m_ValueType)
							{
								long double& self = *((long double*)pPointerValue);
								self = 0;
							}
							else if(BOOL == m_ValueType)
							{
								bool& self = *((bool*)pPointerValue);
								self = false;
							}
							break;
						}
						case INT:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self *= geter;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self *= (long long)geter;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self *= std::stoll(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long long& self = *((long long*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case DOUBLE:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long double& self = *((long double*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self *= (long double)geter;
									break;
								}
								case DOUBLE:
								{
									long double& self = *((long double*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self *= geter;
									break;
								}
								case STRING:
								{
									long double& self = *((long double*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self *= std::stold(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long double& self = *((long double*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case STRING:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									std::string& self = *((std::string*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber * geter;
									}
									catch (...)
									{
									}
									break;
								}
								case DOUBLE:
								{
									std::string& self = *((std::string*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber * geter;
									}
									catch (...)
									{
									}
									break;
								}
								case STRING:
								{
									std::string& self = *((std::string*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										long double geterNumber = std::stold(geter);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber * geterNumber;
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									std::string& self = *((std::string*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self == "true")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										if (geter)
										{
											Boolself = true;
										}
										else
										{
											Boolself = false;
										}
									}
									else if (self == "false")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										Boolself = false;
									}
									break;
								}
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									bool& self = *((bool*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									if (!self && geter == 0)
									{
										self = false;
									}
									break;
								}
								case DOUBLE:
								{
									bool& self = *((bool*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									if (self && geter == 0.0)
									{
										self = false;
									}
									break;
								}
								case STRING:
								{
									bool& self = *((bool*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									if (self && geter == "false")
									{
										self = false;
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									bool& self = *((bool*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self && !geter)
									{
										self = false;
									}
									break;
								}
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator/= (json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)value.m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							m_ValueType = value.m_ValueType;
							Create (value);
							if (INT == m_ValueType)
							{
								long long& self = *((long long*)pPointerValue);
								self = 0;
							}
							else if(DOUBLE == m_ValueType)
							{
								long double& self = *((long double*)pPointerValue);
								self = 0;
							}
							else if(BOOL == m_ValueType)
							{
								bool& self = *((bool*)pPointerValue);
								self = false;
							}
							break;
						}
						case INT:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self /= geter;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self /= (long long)geter;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self /= std::stoll(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long long& self = *((long long*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case DOUBLE:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long double& self = *((long double*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self /= (long double)geter;
									break;
								}
								case DOUBLE:
								{
									long double& self = *((long double*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									self /= geter;
									break;
								}
								case STRING:
								{
									long double& self = *((long double*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self /= std::stold(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long double& self = *((long double*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case STRING:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									std::string& self = *((std::string*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber / geter;
									}
									catch (...)
									{
									}
									break;
								}
								case DOUBLE:
								{
									std::string& self = *((std::string*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber / geter;
									}
									catch (...)
									{
									}
									break;
								}
								case STRING:
								{
									std::string& self = *((std::string*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										long double geterNumber = std::stold(geter);
										Clean();
										m_ValueType = DOUBLE;
										long double& doubleSelf = *((long double*)pPointer);
										doubleSelf = selfNumber / geterNumber;
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									std::string& self = *((std::string*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self == "true")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										if (geter)
										{
											Boolself = true;
										}
										else
										{
											Boolself = false;
										}
									}
									else if (self == "false")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										Boolself = false;
									}
									break;
								}
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									bool& self = *((bool*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									if (!self && geter == 0)
									{
										self = false;
									}
									break;
								}
								case DOUBLE:
								{
									bool& self = *((bool*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									if (self && geter == 0.0)
									{
										self = false;
									}
									break;
								}
								case STRING:
								{
									bool& self = *((bool*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									if (self && geter == "false")
									{
										self = false;
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									bool& self = *((bool*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self && !geter)
									{
										self = false;
									}
									break;
								}
							}
							break;
						}
					}
					return *this;
				}
				json_node& operator%= (json_node& value)
				{
					void* pPointer = (void*)m_ValueBuffer;
					void* pPointerValue = (void*)value.m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							Clean();
							m_ValueType = value.m_ValueType;
							Create (value);
							if (INT == m_ValueType)
							{
								long long& self = *((long long*)pPointerValue);
								self = 0;
							}
							else if(DOUBLE == m_ValueType)
							{
								long double& self = *((long double*)pPointerValue);
								self = 0;
							}
							else if(BOOL == m_ValueType)
							{
								bool& self = *((bool*)pPointerValue);
								self = false;
							}
							break;
						}
						case INT:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self %= geter;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self %= (long long)geter;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self %= std::stoll(geter);
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long long& self = *((long long*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case DOUBLE:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self %= geter;
									m_ValueType = INT;
									break;
								}
								case DOUBLE:
								{
									long long& self = *((long long*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									self %= geter;
									m_ValueType = INT;
									break;
								}
								case STRING:
								{
									long long& self = *((long long*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										self %= std::stoll(geter);
										m_ValueType = INT;
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									long double& self = *((long double*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (!geter)
									{
										self = 0;
									}
									break;
								}
							}
							break;
						}
						case STRING:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									std::string& self = *((std::string*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									try
									{
										long double selfNumber = std::stold(self);
										Clean();
										m_ValueType = INT;
										long long& doubleSelf = *((long long*)pPointer);
										doubleSelf = (long long)selfNumber % (long long)geter;
									}
									catch (...)
									{
									}
									break;
								}
								case DOUBLE:
								{
									std::string& self = *((std::string*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									try
									{
										long long selfNumber = std::stoll(self);
										Clean();
										m_ValueType = INT;
										long long& doubleSelf = *((long long*)pPointer);
										doubleSelf = (long long)selfNumber % (long long)geter;
									}
									catch (...)
									{
									}
									break;
								}
								case STRING:
								{
									std::string& self = *((std::string*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									try
									{
										long long selfNumber = std::stoll(self);
										long long geterNumber = std::stoll(geter);
										Clean();
										m_ValueType = INT;
										long long& doubleSelf = *((long long*)pPointer);
										doubleSelf = (long long)selfNumber % (long long)geterNumber;
									}
									catch (...)
									{
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									std::string& self = *((std::string*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self == "true")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										if (geter)
										{
											Boolself = true;
										}
										else
										{
											Boolself = false;
										}
									}
									else if (self == "false")
									{
										Clean();
										m_ValueType = BOOL;
										bool& Boolself = *((bool*)pPointerValue);
										Boolself = false;
									}
									break;
								}
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							switch (value.m_ValueType)
							{
								case NONE:
								{
									break;
								}
								case INT:
								{
									bool& self = *((bool*)pPointer);
									long long& geter = *((long long*)pPointerValue);
									if (!self && geter == 0)
									{
										self = false;
									}
									break;
								}
								case DOUBLE:
								{
									bool& self = *((bool*)pPointer);
									long double& geter = *((long double*)pPointerValue);
									if (self && geter == 0.0)
									{
										self = false;
									}
									break;
								}
								case STRING:
								{
									bool& self = *((bool*)pPointer);
									std::string& geter = *((std::string*)pPointerValue);
									if (self && geter == "false")
									{
										self = false;
									}
									break;
								}
								case JSON:
								{
									break;
								}
								case VECTOR:
								{
									break;
								}
								case BOOL:
								{
									bool& self = *((bool*)pPointer);
									bool& geter = *((bool*)pPointerValue);
									if (self && !geter)
									{
										self = false;
									}
									break;
								}
							}
							break;
						}
					}
					return *this;
				}
				json_node operator+ (json_node& value)
				{
					json_node Rt(*this);
					Rt += value;
					return Rt;
				}
				json_node operator- (json_node& value)
				{
					json_node Rt(*this);
					Rt -= value;
					return Rt;
				}
				json_node operator* (json_node& value)
				{
					json_node Rt(*this);
					Rt *= value;
					return Rt;
				}
				json_node operator/ (json_node& value)
				{
					json_node Rt(*this);
					Rt /= value;
					return Rt;
				}
				json_node operator% (json_node& value)
				{
					json_node Rt(*this);
					Rt %= value;
					return Rt;
				}
				void toString(std::string& strGet, int iFormat = -1)
				{
					std::string strFormat = "";
					if (-1 != iFormat)
					{
						for (int i = 0; i < iFormat; ++i)
						{
							strFormat += '\t';
						}
					}
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							strGet += "null";
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							strGet += std::to_string(self);
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							strGet += std::to_string(self);
							break;
						}
						case STRING:
						{
							std::string& self = *((std::string*)pPointer);
							strGet += '"';
							for (size_t i = 0; i < self.length(); ++i)
							{
								if ('"' == self[i])
								{
									strGet += '\\';
								}
								strGet += self[i];
							}
							strGet += '"';
							break;
						}
						case JSON:
						{
							json_nodeMap& self = *((json_nodeMap*)pPointer);
							if (-1 == iFormat)
							{
								strGet += '{';
							}
							else
							{
								if (0 != iFormat)
								{
									if ('\t' == strGet[strGet.length() -1])
									{
										strGet += "{\n";
									}
									else
									{
										strGet += "\n";
										strGet += strFormat + "{\n";
									}
								}
								else
								{
									strGet += "{\n";
								}
							}
							bool bisFirst = true;
							for (typename json_nodeMap::iterator it = self.begin(); it != self.end(); ++it)
							{
								if (!bisFirst)
								{
									strGet += ',';
									if (-1 != iFormat)
									{
										strGet += '\n';
									}
								}
								if (-1 != iFormat)
								{
									strGet += strFormat + '\t';
								}
								strGet += '"';
								for (size_t i = 0; i < it->first.length(); ++i)
								{
									if ('"' == (it->first)[i])
									{
										strGet += '\\';
									}
									strGet += (it->first)[i];
								}
								strGet += "\":";
								int iNextFormat = -1;
								if (-1 != iFormat)
								{
									strGet += " ";
									iNextFormat = iFormat + 1;
								}
								it->second.toString(strGet, iNextFormat);
								bisFirst = false;
							}
							if (-1 == iFormat)
							{
								strGet += '}';
							}
							else
							{
								strGet += '\n';
								strGet += strFormat + "}";
							}
							break;
						}
						case VECTOR:
						{
							json_nodeArray& self = *((json_nodeArray*)pPointer);
							if (-1 == iFormat)
							{
								strGet += '[';
							}
							else
							{
								if (0 != iFormat)
								{
									if ('\t' == strGet[strGet.length() -1])
									{
										strGet += "[\n";
									}
									else
									{
										strGet += "\n";
										strGet += strFormat + "[\n";
									}
								}
								else
								{
									strGet += "[\n";
								}
							}
							bool bisFirst = true;
							for (size_t i = 0; i < self.size(); ++i)
							{
								if (!bisFirst)
								{
									strGet += ',';
									if (-1 != iFormat)
									{
										strGet += '\n';
									}
								}
								if (-1 != iFormat)
								{
									strGet += strFormat + '\t';
								}
								int iNextFormat = -1;
								if (-1 != iFormat)
								{
									iNextFormat = iFormat + 1;
								}
								self[i].toString(strGet, iNextFormat);
								bisFirst = false;
							}
							if (-1 == iFormat)
							{
								strGet += ']';
							}
							else
							{
								strGet += '\n';
								strGet += strFormat + "]";
							}
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							if (self)
							{
								strGet += "true";
							}
							else
							{
								strGet += "false";
							}
						}
					}
				}
				json_node& operator << (std::string strFilePath)
				{
					std::ifstream ifs(strFilePath.c_str());
					if (ifs)
					{
						std::stringstream ss;
						ss << ifs.rdbuf();
						this->fromString(ss.str());
					}
					return *this;
				}
				void operator >> (std::string strFilePath)
				{
					std::ofstream ofs(strFilePath.c_str());
					if (ofs)
					{
						std::string strGet;
						this->toString(strGet, 0);
						ofs << strGet << std::endl;
					}
				}
				unsigned char* getRef()
				{
					return m_ValueBuffer;
				}
				const long long& getRefInt()
				{
					void* pPointer = nullptr;
					if (m_ValueType == INT)
					{
						pPointer = (void*)m_ValueBuffer;
					}
					return *((long long*)pPointer);
				}
				const bool& getRefBool()
				{
					void* pPointer = nullptr;
					if (m_ValueType == BOOL)
					{
						pPointer = (void*)m_ValueBuffer;
					}
					return *((bool*)pPointer);
				}
				const long double& getRefDouble()
				{
					void* pPointer = nullptr;
					if (m_ValueType == DOUBLE)
					{
						pPointer = (void*)m_ValueBuffer;
					}
					return *((long double*)pPointer);
				}
				const std::string& getRefString()
				{
					void* pPointer = nullptr;
					if (m_ValueType == STRING)
					{
						pPointer = (void*)m_ValueBuffer;
					}
					return *((std::string*)pPointer);
				}
				const char* getCharString()
				{
					const std::string& str = getRefString();
					return str.c_str();
				}
				long long getInt()
				{
					void* pPointer = (void*)m_ValueBuffer;
					long long lRt = 0;
					switch (m_ValueType)
					{
						case NONE:
						{
							lRt = 0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							lRt = self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							lRt = self;
							break;
						}
						case STRING:
						{
							std::string& self = (*(std::string*)pPointer);
							try
							{
								lRt = std::stoll(self);
							}
							catch (...)
							{
			
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							lRt = self ? 1 : 0;
							break;
						}
					}
					return lRt;
				}
				bool getBool()
				{
					void* pPointer = (void*)m_ValueBuffer;
					bool lRt = 0;
					switch (m_ValueType)
					{
						case NONE:
						{
							lRt = false;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							lRt = (self == 0) ? false : true;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							lRt = (self == 0.00) ? false : true;
							break;
						}
						case STRING:
						{
							std::string& self = (*(std::string*)pPointer);
							lRt = (self == "true") ? true : false;
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							lRt = self;
							break;
						}
					}
					return lRt;
				}
				long double getDouble()
				{
					void* pPointer = (void*)m_ValueBuffer;
					long double lRt = 0.0;
					switch (m_ValueType)
					{
						case NONE:
						{
							lRt = 0.0;
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							lRt = self;
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							lRt = self;
							break;
						}
						case STRING:
						{
							std::string& self = (*(std::string*)pPointer);
							try
							{
								lRt = std::stold(self);
							}
							catch (...)
							{
			
							}
							break;
						}
						case JSON:
						{
							break;
						}
						case VECTOR:
						{
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							lRt = self ? 1.0 : 0.0;
							break;
						}
					}
					return lRt;
				}
				void getString(std::string& strGet)
				{
					void* pPointer = (void*)m_ValueBuffer;
					switch (m_ValueType)
					{
						case NONE:
						{
							strGet = "";
							break;
						}
						case INT:
						{
							long long& self = *((long long*)pPointer);
							try
							{
								strGet = std::to_string(self);
							}
							catch (...)
							{
							}
							break;
						}
						case DOUBLE:
						{
							long double& self = *((long double*)pPointer);
							try
							{
								strGet = std::to_string(self);
							}
							catch (...)
							{
							}
							break;
						}
						case STRING:
						{
							strGet = ((std::string*)pPointer)->c_str();
							break;
						}
						case JSON:
						{
							this->toString(strGet);
							break;
						}
						case VECTOR:
						{
							this->toString(strGet);
							break;
						}
						case BOOL:
						{
							bool& self = *((bool*)pPointer);
							strGet = self ? "true" : "false";
							break;
						}
					}
				}
				ValueType getType()
				{
					return m_ValueType;
				}
				bool fromString(const char* pszBuff)
				{
					size_t uChildSize = 0;
					if (!fromStringInternal(pszBuff, uChildSize))
					{
						return false;
					}
					pszBuff += uChildSize;
					size_t i = 0;
					while (0 != pszBuff[i])
					{
						if (' ' != pszBuff[i] &&
								'\n' != pszBuff[i] &&
								'\r' != pszBuff[i] &&
								'\t' != pszBuff[i])
						{
							Reset(NONE);
							return false;
						}
						++i;
					}
					return true;
				}
				bool fromString(const std::string& Str)
				{
					return fromString(Str.c_str());
				}
			};
			inline std::ostream& operator<< (std::ostream& out, json_node& JsonObj)
			{
				std::string strGet = "";
				JsonObj.toString(strGet, 0);
				out << strGet;
				return out;
			}
			inline std::istream& operator>> (std::istream& in, json_node& JsonObj)
			{
				std::string strGet;
				in >> strGet;
				JsonObj.fromString(strGet);
				return in;
			}
		}
	}
}
#endif




