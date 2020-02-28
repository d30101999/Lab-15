#pragma once
#include <string.h>


#include "MakeList.h"
	typedef struct stat
	{
		int count;
		unsigned char c;
		struct stat * left;
		struct stat * right;
	} stat;
	stat * Tree;
	int TreeLen;

	void freeTree()
	{
		if (TreeLen) free(Tree);
		TreeLen = 0;
	}

	void MakeStat()
	{
		freeTree();
		Tree = malloc(256*sizeof(stat));
		TreeLen = 256;
		for (int k = 0; k < 256; k++)
		{
			Tree[k].count = 0;
			Tree[k].c = k;
			Tree[k].left = Tree[k].right = NULL;
		}
		for (int k = 0; k < sourceLen; k++) Tree[source[k]].count++;
	}


	void SortStat()
	{
		for (int k = 0; k < TreeLen; k++)
		{
			int best = k;
			for (int i = k + 1; i < TreeLen; i++)
				if (Tree[i].count < Tree[best].count)
					best = i;
			if (best != k)
			{
				stat temp = Tree[best];
				Tree[best] = Tree[k];
				Tree[k] = temp;
			}
		}
	}


	void Push(stat SubTree)
	{
		Tree = (stat*)realloc(Tree, (sizeof(stat)*(TreeLen + 1)));
		Tree[TreeLen++] = SubTree;
	}


	stat Pop()
	{
		stat Result = Tree[0];
		for (int k = 0; k < TreeLen - 1; k++)
			Tree[k] = Tree[k + 1];
		Tree = (stat*)realloc(Tree, (sizeof(stat)*(--TreeLen)));
		return Result;
	}


	void CleanTree()
	{
		while (Tree[0].count == 0 && TreeLen>0) Pop();
	}


	void MakeTree()
	{
		while (TreeLen > 1)
		{

			stat * E1 = malloc(sizeof(stat));
			*E1 = Pop();

			stat * E2 = malloc(sizeof(stat));
			*E2 = Pop();

			stat E;
			E.c = 0;
			E.count = E1->count + E2->count; //Sum of weights
			E.left = E1;
			E.right = E2;

			Push(E);

			SortStat();
		}
	}


	void PackStat()
	{

		tableLen = TreeLen * 4;
		table = (unsigned char*)realloc(table, tableLen);
		for (int k = 0; k < TreeLen; k++)
		{
			table[4 * k] = Tree[k].c;
			int x = Tree[k].count;
			table[4 * k + 3] = x & 0xFF;
			x >>= 8;
			table[4 * k + 2] = x & 0xFF;
			x >>= 8;
			table[4 * k + 1] = x & 0xFF;
		}
	}


	void UnpackStat()
	{
		TreeLen = tableLen / 4;
		Tree = (stat*)realloc(Tree, sizeof(stat)*TreeLen);
		for (int k = 0; k < TreeLen; k++)
		{
			Tree[k].c = table[4 * k];
			Tree[k].left = Tree[k].right = NULL;
			Tree[k].count = 0;
			Tree[k].count += table[4 * k + 1];
			Tree[k].count <<= 8;
			Tree[k].count += table[4 * k + 2];
			Tree[k].count <<= 8;
			Tree[k].count += table[4 * k + 3];
		}
	}


	typedef struct CodeTable
	{
		int len;
		bool path[256];
	} CodeTable;
	CodeTable codetable[256];
	CodeTable current;

	void MakeTableFromTree(stat * subtree)
	{
		if (!subtree)
			return;
		if (subtree->left == NULL && subtree->right == NULL)
		{

			codetable[subtree->c] = current;
			return;
		}


		current.path[current.len++] = false;
		MakeTableFromTree(subtree->left);
		current.len--;
		current.path[current.len++] = true;
		MakeTableFromTree(subtree->right);
		current.path[current.len] = (bool)2;
		current.len--;
	}


	void PackCurrentPath()
	{
		while (current.len >= 8)
		{

			unsigned char c = 0;
			for (int k = 0; k < 8; k++)
			{
				c <<= 1;
				if (current.path[k]) c++;
			}

			current.len -= 8;

			for (int k = 0; k < current.len; k++)
				current.path[k] = current.path[k + 8];

			target = (unsigned char*)realloc(target, targetLen + 1);
			target[targetLen++] = c;
		}
	}


	void AddToTarget(unsigned char c)
	{
		for (int k = 0; k < codetable[c].len; k++)
			current.path[current.len++] = codetable[c].path[k];
		PackCurrentPath();
	}


	void Pack()
	{
		freetarget(); //clear result
		MakeStat(); //Type statistics by text
		SortStat(); //Put statistics in order
		CleanTree();
		PackStat();	//Pack the tree in a table while preserving only information useful for decoding
		//Create a table here with no extra characters
		MakeTree();

		current.len = 0;

		for (int k = 0; k < 256; k++)
		{
			codetable[k].len = 0;
			for (int i = 0; i < sizeof(codetable[k].path); i++)
				codetable[k].path[i] = false;
		}
		MakeTableFromTree(&Tree[0]);
		current.len = 0;

		for (int k = 0; k < sourceLen; k++)
			AddToTarget(source[k]);
		for (int k = current.len; k < 9; k++)
		{
			current.path[k] = true;
		};
		current.len = 9;
		PackCurrentPath();
	}

	void Unpack()
	{
		freesource();
		UnpackStat();
		MakeTree();
		stat * decoder = &Tree[0];
		for (int k = 0; k < targetLen; k++)
		{
			unsigned char c = target[k];

			for (int b = 0; b < 8; b++)
			{

				bool x = (c >= 128);
				c <<= 1;
				if (x)
					decoder = decoder->right;
				else
					decoder = decoder->left;

				if (decoder->left == NULL && decoder->right == NULL)
				{
					source = (unsigned char *)realloc(source, sourceLen + 1);
					source[sourceLen++] = decoder->c;

					decoder = &Tree[0];
				}
			}
		}
	}

