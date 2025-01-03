/*
MIT License

Copyright (c) 2025 ARTELE

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>
#include <format>
namespace bml
{	
	template<class T>
	concept ValueNodeBaseType = (
		std::is_same_v<T, float> ||
		std::is_same_v<T, double> ||
		std::is_same_v<T, long double> ||
		std::is_same_v<T, int> ||
		std::is_same_v<T, long> ||
		std::is_same_v<T, long long> ||
		std::is_same_v<T, unsigned long> ||
		std::is_same_v<T, unsigned int> ||
		std::is_same_v<T, unsigned long long> ||
		std::is_same_v<T, std::string>);

	class ValueNode
	{
		std::string mName;
		std::string mValue;
		std::vector<std::shared_ptr<ValueNode>> mChilds;
	public:
		ValueNode() {}
		ValueNode(const std::string& name) : mName(name) {}
		template<ValueNodeBaseType T> ValueNode(const std::string& name, T value) : mName(name), mValue(std::to_string(value)) {}
		template<> ValueNode(const std::string& name, std::string value) : mName(name), mValue(value) {}
		const std::string& Name() { return mName; }
		size_t ChildCount() const { return mChilds.size(); }
		std::shared_ptr<ValueNode> Child(size_t i) { return (i >= 0 || i < mChilds.size()) ? mChilds[i] : nullptr; }
		void AddChild(const std::shared_ptr<ValueNode>& node) { if (node) mChilds.push_back(node); }
		std::shared_ptr<ValueNode> Find(const std::string& name)
		{
			auto iter = std::find_if(mChilds.begin(), mChilds.end(), [&](const std::shared_ptr<ValueNode>& node){ return node->Name() == name; });
			return iter == mChilds.end() ? nullptr : *iter;
		}
		template<ValueNodeBaseType T> T Value() const { return T(mValue); }
		template<> int Value() const { return std::stoi(mValue); }
		template<> unsigned int Value() const { return (unsigned int)std::stoi(mValue); }
		template<> long Value() const { return std::stol(mValue); }
		template<> unsigned long Value() const { return std::stoul(mValue); }
		template<> long long Value() const { return std::stoll(mValue); }
		template<> unsigned long long Value() const { return std::stoull(mValue); }
		template<> float Value() const { return std::stof(mValue); }
		template<> double Value() const { return std::stod(mValue); }
		template<> long double Value() const { return std::stold(mValue); }
		template<ValueNodeBaseType T> std::pair<std::string, T> NameAndValue() { return std::make_pair(Name(), Value<T>()); }
	};

	class Serialize
	{
		static void ToFile(const std::shared_ptr<ValueNode>& root, std::ofstream& out, size_t floor)
		{
			if (root)
			{
				std::string outData(floor * 2, ' ');
				auto [name, value] = root->NameAndValue<std::string>();
				outData += std::format("{}: {} \n", name, value);
				out.write(outData.c_str(), outData.size());
				for (int i = 0; i < root->ChildCount(); i++)
				{
					ToFile(root->Child(i), out, floor + 1);
				}
			}
		}

		static std::pair<std::string, std::string> ParseLine(const std::string& data)
		{
			std::string lineData = data;
			std::replace(lineData.begin(), lineData.end(), ':', ' ');
			std::stringstream ss(lineData);
			std::vector<std::string> tokens;
			std::string token;
			while (std::getline(ss, token, ' '))
			{
				if (!token.empty())
				{
					tokens.push_back(token);
				}
			}
			std::string name, value;
			if (!tokens.empty())
			{
				name = tokens.front();
				if (tokens.size() > 1)
				{
					value = tokens.back();
				}
			}
			return std::make_pair(name, value);
		}

	public:
		static void ToTextFile(const std::shared_ptr<ValueNode>& root, const char* filename)
		{
			std::ofstream out(filename, std::ios::out);
			ToFile(root, out, 0);
			out.close();
		}

		static std::shared_ptr<ValueNode> FromTextFile(const char* filename)
		{
			std::ifstream in(filename, std::ios::in);
			if (!in.is_open()) return nullptr;
			std::string lineData;
			std::vector<std::shared_ptr<ValueNode>> nodeStack;
			while (std::getline(in, lineData))
			{
				size_t index = (lineData.find_first_not_of(' '));
				if ((index & 1) == 1)  return nullptr;
				auto [name, value] = ParseLine(lineData);
				while ((index >> 1) + 1 <= nodeStack.size())
				{
					nodeStack.pop_back();
				}
				auto node = std::make_shared<ValueNode>(name, value);
				if (!nodeStack.empty())
				{
					nodeStack.back()->AddChild(node);
				}
				nodeStack.push_back(node);
			}
			return nodeStack.empty() ? nullptr : nodeStack.front();
		}
	};
}