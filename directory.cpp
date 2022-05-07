#include <unistd.h>
#include <string>
#include <iostream>
#include <dirent.h> // allows for struct dirent (directory entry)
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <algorithm>
using namespace std; 

enum DTYPE{DIRECTORY, FILETYPE, SYMBOLIC_LINK, OTHER};

struct SmartEntry{
		dirent* d;
		DTYPE type;
		bool isDotFile;
	};
	

//TODO: Create a class that allows you to reference the flags that during the execution of the program
class DIRControls
{
	public:
		DIRControls(int ia, int iff, int id, int il, string dir_name)
		{
			a = ia;
          	f = iff;
          	d = id;
          	l = il;
			directory_name = dir_name;
		}
	//private:
		int a, f, d, l;
		string directory_name;
};

string toLowerCase(string str) //https://helloacm.com/c-c-coding-exercisse-how-to-implement-tolowercase-function/
{
	auto r = str;
	for (auto i = 0; i < str.length(); ++ i) {
		if (r[i] >= 'A' && r[i] <= 'Z') {
			r[i] += 32;
		} 
	}
	return r;
}

void selectionSort(vector<SmartEntry> &entries) // adapted from https://www.geeksforgeeks.org/selection-sort/
{ 
    int i, j, min_idx; 
  
    // One by one move boundary of unsorted subarray 
    for (i = 0; i < entries.size()-1; i++) 
    { 
        // Find the minimum element in unsorted array 
        min_idx = i; 
        for (j = i+1; j < entries.size(); j++) 
        if (toLowerCase(string(entries[j].d->d_name)).compare(toLowerCase(string(entries[min_idx].d->d_name))) < 0) 
            min_idx = j; 
  
        // Swap the found minimum element with the first element 
		SmartEntry temp = entries[i];
		entries[i] = entries[min_idx];
		entries[min_idx] = temp;
    } 
} 

void print(SmartEntry s, int a, int f, int d, int l)
{
	struct stat sb;
	struct group *g;
	struct passwd *pw;

	bool printDotFile = false;

	switch(l)
	{
		case 1: // LONG FORMAT
			if(!s.isDotFile || a == 1)
			{
				if (stat(s.d->d_name, &sb) == -1) 
				{
        			perror("stat");
        			exit(EXIT_FAILURE);
    			}
				pw = getpwuid(sb.st_uid);
				g = getgrgid(sb.st_gid);
				//taken from https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
				printf( (S_ISDIR(sb.st_mode)) ? "d" : "-");
				printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
				printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
				printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
				printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
				printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
				printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
				printf( (sb.st_mode & S_IROTH) ? "r" : "-");
				printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
				printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
				string time = ctime(&sb.st_mtime);
				time.erase(std::remove(time.begin(), time.end(), '\n'), time.end()); //https://www.systutorials.com/how-to-remove-newline-characters-from-a-string-in-c/
				cout << "\t" << (long)sb.st_nlink << "\t" << (string) pw->pw_name << "\t" << (string) g->gr_name << "\t" << (long long) sb.st_size << "\t" << time << "\t" << s.d->d_name;
				if(s.type != SYMBOLIC_LINK || f == 0)
					cout << endl;
			}
			break;

		default: //SHORT FORMAT
			if(!s.isDotFile || a == 1)
			{
				cout << "\t" << s.d->d_name;
				if(s.type != SYMBOLIC_LINK || f == 0)
					cout << endl;
					
			}
			break;
	}	
}


void recursive(DIRControls* directory);


int main(int argc, char** argv)
{
	vector<DIRControls*> arguments;
	opterr = 0; //will not print error messages for unknown option characters/missing arguments
	int c;
	int a = 0, f = 0, d = 0, l = 0;
	string dir_name;
	bool newSequence = true, noGetOpts = true;
	
	while((c = getopt(argc, argv, "afdlh")) != -1)
	{ 
		noGetOpts = false;
      	if(newSequence)
        {
          		a = 0;	//reset flags for opt args
				f = 0;
				d = 0;
				l = 0;
          		newSequence = false;
        }
		switch(c)
		{
			case 'a':
				a = 1;
				break;
			case 'f':
				f = 1;
				break;
			case 'd':
				d = 1;
				break;
			case 'l':
				l = 1;
				break;
			case 'h':
				cout << "Usage: dirls [(-[adflh]+ (dir)]*" 	<< endl;
				cout << "\t -a: include dot files" 			<< endl;
				cout << "\t -f: follow symbolic links" 		<< endl;
				cout << "\t -d: only this directory" 		<< endl;
				cout << "\t -l: long form" 					<< endl;
				cout << "\t -h: prints this message"		<< endl;
				break;
			case '?':
				break;
        }
		/*if(optind == argc - 1)	//we have reached the last argument
		{
			if(a == 1 || f == 1 || d == 1 || l == 1)
				arguments.push_back(new DIRControls(a,f,d,l,getcwd(NULL, 0)));
		}*/
		if(optind < argc)
		{
			for(int i = optind; i < argc && argv[i][0] != '-'; i ++) //next argument is not an option
			{
				dir_name = argv[i];
				arguments.push_back(new DIRControls(a,f,d,l, string(get_current_dir_name()) + "/" + dir_name));
				newSequence = true;
			}
		}
		else
		{
			arguments.push_back(new DIRControls(a,f,d,l,string(get_current_dir_name())));
		}
		
	}
	if(noGetOpts)
	{
		for(int i = 1; i < argc; i ++)
		{
			arguments.push_back(new DIRControls(a,f,d,l,string(get_current_dir_name()) + "/" + string(argv[i])));
			noGetOpts = false;
		}
	}

	if(noGetOpts)
		arguments.push_back(new DIRControls(a,f,d,l,string(get_current_dir_name())));

	for(DIRControls *d : arguments)
	{
		recursive(d);
		cout << endl;
	}
}


void recursive(DIRControls* directory)
{
	chdir(directory->directory_name.c_str());
	vector<SmartEntry> entries;
	queue<DIRControls*> directories;
	DIR* dir_pointer = opendir(directory->directory_name.c_str()); //convert directory_name to c_str() and opendir
	if(dir_pointer == NULL)
	{
		cout << "DIR name invalid" << endl;
		return;
	}
	cout << endl;
	cout << directory->directory_name << ":" << endl;

	dirent* dir_entry;
	SmartEntry s;
	while(dir_entry = readdir(dir_pointer))
	{
		s.d = dir_entry;
		if(int(dir_entry->d_type) == 4)
			s.type = DIRECTORY;
		else if(int(dir_entry->d_type) == 8)
			s.type = FILETYPE;
		else if(int(dir_entry->d_type) == 10)
			s.type = SYMBOLIC_LINK;
		else
			s.type = OTHER;

		if(dir_entry->d_name[0] == '.')
			s.isDotFile = true;
		else
			s.isDotFile = false;

		entries.push_back(s);
	}

	selectionSort(entries);

	for(SmartEntry s : entries)
	{
		if(s.type == DIRECTORY)
		{
			if(directory->d != 1 && !s.isDotFile)
				directories.push(new DIRControls(directory->a, directory->f, directory->d, directory->l, directory->directory_name + "/" + s.d->d_name));
				
			print(s, directory->a, directory->f, directory->d, directory->l);
		}
		else if(s.type == FILETYPE)
		{
			print(s, directory->a, directory->f, directory->d, directory->l);
		}
		else if(s.type == SYMBOLIC_LINK)
		{	
			print(s, directory->a, directory->f, directory->d, directory->l);
			if(directory->f == 1)	
			{
				char buf[PATH_MAX];
				char *res = realpath(s.d->d_name, buf);
				
				cout << "->" << string(buf) << endl;
			}
	
		}
		else
		{
			cout << "Unknown File Type" << endl;
		}
	}
	closedir(dir_pointer);

	while(!directories.empty())
	{
		recursive(directories.front());
		directories.pop();
	}
	

	
}