#ifndef _LANGUAGECLASS_H_
#define _LANGUAGECLASS_H_
#include <List.h>
#include <string.h>

#include "PrefData.h"  //by me ;-)

struct lang_struct {
	lang_struct(char *pName, char *pEntry){name = strdup(pName); entry = strdup(pEntry); }
	char	*name,
			*entry;
};

class LanguageClass {
public:
		LanguageClass();
				
		char	*get(char*);
		void InitLang();

private:

		bool	load();
		void	set(char*, char*, bool pCheck = false);
	
	BList	LList;
	int		size;
};

extern LanguageClass Language;

#endif
