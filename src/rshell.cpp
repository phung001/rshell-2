#include <string.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

//takes in 3 bools and 1 string 
//outputs the number for the closest connector
//sets one or none of the 3 connector bools
int connector(bool &next, bool &orr, bool &andd, string const cmd)
{
	int nexti = cmd.find(";");
	int orri = cmd.find("||");
	int anddi = cmd.find("&&");
	
	if (nexti == -1 && orri == -1 && anddi == -1)
	{
		return -1;
	}
	else if (nexti == -1 && orri >= 0 && anddi >= 0)
	{
		if (orri < anddi)
		{
			orr = true;
			return orri;
		}
		else
		{
			andd = true;
			return anddi;
		}
	}
	else if (nexti >= 0 && orri == -1 && anddi >= 0)
	{	
		if (nexti < anddi)
		{
			next = true;
			return nexti;
		}
		else
		{
			andd = true;
			return anddi;
		}
	}
	else if (nexti >= 0 && orri >= 0 && anddi == -1)
	{
		if (nexti < orri)
		{
			next = true;
			return nexti;
		}
		else 
		{
			orr = true;
			return orri;
		}
	}
	else if (nexti >= 0 && orri == -1 && anddi == -1)
	{
		next = true;
		return nexti;
	}
	else if (nexti == -1 && orri >= 0 && anddi == -1)
	{
		orr = true;
		return orri;
	}
	else if (nexti == -1 && orri == -1 && anddi >= 0)
	{
		andd = true;
		return anddi;
	}
	else
	{
		if (nexti < orri)
		{
			if (anddi < nexti)
			{
				andd = true;
				return anddi;
			}
			else
			{
				next = true;
				return nexti;
			}
		}
		else
		{
			if (anddi < orri)
			{
				andd = true;
				return anddi;
			}
			else 
			{
				orr = true;
				return orri;
			}
		}
	}
}
//will fork and run cmd
//will perror if system call faults
//returns -1 if unable to run command else 0
int execb (string in)
{
	pid_t pid = fork();
		if (pid == -1)
			perror("failed to fork");
		else if (pid == 0)
		{
		
			char *arg[in.length()];
			char *temp = strdup(in.c_str());
			arg[0] = strtok(temp, " ");
			char *token = arg[0];
			for (int i = 1; token != NULL; i++)
			{
				arg[i] = strtok(NULL, " ");
				token = arg[i];
			}
			int execs = execvp(arg[0], arg);
			if (execs == -1)
			{
				perror("cannot execute command");
				return -1;
			}
			delete temp;
		}
		else 
		{
			int waits;
			waits = wait(NULL);
			if (waits == -1)
				perror("no child");
		}
	return 0;
}
int main(int argc, char **argv)
{
	string cmd;
	for(;;)
	{
	finish:
		string in;
		cout << "$";
		getline(cin,in);
		bool next = false, orr = false, andd = false;
		int cut = connector(next, orr, andd, in);
		while (cut != -1)
		{	
			if (next)
			{
				cmd = in.substr(0,cut);
				in = in.substr(cut+1, string::npos);
			}
			else
			{	
				cmd = in.substr(0,cut);
				in = in.substr(cut+2, string::npos);
			
			}
			if (orr)
				if (execb(cmd) != -1)
					goto finish;
			if (andd)
				if (execb(cmd) == -1)
					goto finish;
			if (next)
				execb(cmd);

			next = false;
			andd = false;
			orr = false;
			cut = connector(next, orr, andd, in);
		
		}
		execb(in);
	}
	return 0;
}