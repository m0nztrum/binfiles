/* Copyright 2023 punicorn
 * Gitalias a git alias
 */

/* boost includes */
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/value_semantic.hpp>
/* C++ includes */
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <new>
#include <string>
#include <string_view>
#include <vector>
/*C includes*/
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
/* making life easier */
#define null NULL
#define arg(cmd, type) args[cmd].as<type>()

namespace opt = boost::program_options;
using std::string, std::vector, std::string_view, std::cout, std::cerr, std::nothrow, std::endl, std::cin,
    std::ifstream;

/* global varibales -> helps prevents extreme function arguments*/
/* could use a class and functors */
string messagebox, addbox, reponame, repodes, subcommand, Resetcommand,
    command = "[[ -f /bin/git || -f /usr/bin/git ]] ";
bool mode; // for the user request;
vector<string>(Sreset);
struct Trips
{
    // constructor resets everything in struct
    Trips()
    {
        memset(this, 0, sizeof(*this));
    };
    bool commit, message, add, init, branch, switch_, deleteBranch, merge, pull, push, origin, log, status, repoName,
        repoDes, repoMode, verbose, Rreset, git;
};

/* ============== function declarations ================*/
/* append into subcommand [ ret: void ]*/
void Isubcommand(const string_view &s1, const string_view &s2);
/* prints error message e with help message and kills program [ret: void]*/
auto errorT1(const string_view &e) -> void;
/* prints error message e and kills the program  [ ret: void ]*/
auto errorT2(const string_view &e) -> void;
/* check is thing exists at place [ ret: bool ]*/
auto getdir(const string &place, const string_view &thing) -> bool;
/* checks if you have a git repo init [ ret: bool ]*/
auto getGitInfo() -> bool;
/* check for staged files [ ret: bool ]*/
auto Checkadd() -> bool;
/* create an online repository [ ret: void ]*/
auto createOnlineRepo() -> void;
/* runs the command [ ret: void ]*/
auto run(bool v) -> void;
/* parses the struct [ ret: void ] */
auto parse(Trips *t) -> void;

/* ============== Main ============*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        errorT1("No arguments passed...");
    }
    Trips *T = new (nothrow) Trips();
    if (T == null)
    {
        cerr << program_invocation_name << ": Program Crashed...\n"
             << "Err: Trip could not be allocated\n";
        exit(1);
    }
    try
    {
        opt::options_description desc(string(*argv).append(" options"));
        desc.add_options()("help,h", "print this message")("init,i", "init a repository")(
            "commit,c", "add all and commit")("add,a", opt::value<vector<string>>()->multitoken(), "add [files] only")(
            "message,m", opt::value<vector<string>>()->multitoken(),
            "add a message")("branch,b", opt::value<string>()->implicit_value(""),
                             "create a branch")("switch,s", opt::value<string>(), "switch to a branch")(
            "delete,d", opt::value<string>(), "delete a branch")("Merge,M", opt::value<string>(),
                                                                 "merge branch [ branch-name ] with current branch")(
            "Pull,P", opt::value<string>()->implicit_value(""),
            "pull from origin")("push,p", opt::value<string>()->implicit_value(""), "push into origin")(
            "Clone,C", opt::value<vector<string>>()->multitoken(), "clone a repository with given \"user/repo-name\"")(
            "Request,R", opt::value<string>()->default_value("https"), "Protocol to use when cloning, [ https/ssh ]")(
            "verbose,v", "print out parsed code")("log,l", "show log files")("Status,S", "show statuts")(
            "origin,o", opt::value<string>(), "add an origin")("repo,r", opt::value<string>(&reponame),
                                                               "name for creating an online repo * [i]")(
            "Des,D", opt::value<string>(&repodes), "description for the online repo * [ii]")(
            "type,t", opt::value<bool>(&mode)->value_name("bool"), "if repo should be private * [iii]")(
            "undo,u", opt::value<vector<string>>()->multitoken(),
            "*Reset back to a commit ,eg gitalias -u "
            "[ hard / soft / mixed ] "
            "[ number of commits to reset back ]")("git,g", opt::value<vector<string>>()->multitoken(),
                                                   "append git commands eg [ gitalias -g \" git ... \" ] for extra "
                                                   "commands "
                                                   "append git to it eg: [ ga -G \"add .\" \" git commit -m 'foo' \" "
                                                   "... ] "
                                                   "]")("Grab,G", opt::value<string>(),
                                                        "grab a specific folder from a github repo");

        opt::variables_map args;
        opt::store(opt::command_line_parser(argc, argv)
                       .options(desc)
                       .style(opt::command_line_style::default_style | opt::command_line_style::allow_sticky)
                       .run(),
                   args);
        opt::notify(args);
        if (args.count("help"))
        {
            /* appending more info into the string ::
            only needed when theres a command for only shorthands
            stringstream stream;
            stream << desc;
            string helpMsg = stream.str();
            boost::algorithm::replace_all(helpMsg, "--", "-");
            boost::algorithm::replace_all(helpMsg, "[ -", "[ --");
            cout << helpMsg << "\n";
            */
            cout << desc << "\n";
            exit(0);
        }
        /* Check for trips */
        if (args.count("commit"))
        {
            T->commit = true;
        }

        if (args.count("message"))
        {
            T->message = true;
            for (vector<string>::const_iterator i = args["message"].as<vector<string>>().begin();
                 i != args["message"].as<vector<string>>().end(); i++)
            {
                messagebox += " -m '";
                messagebox += *i;
                messagebox += "' ";
            }
        }

        if (args.count("init"))
        {
            T->init = true;
        }
        if (args.count("add"))
        {
            T->add = true;
            addbox += " && git add ";
            cout << "Files added: ";
            for (auto i : args["add"].as<vector<string>>())
            {
                if (i == ".")
                {
                    cout << "all ";
                }
                else
                {
                    cout << i << " ";
                }
                addbox += i;
                addbox += " ";
            }
            cout << endl;
        }
        if (args.count("branch"))
        {
            T->branch = true;
            Isubcommand(" && git branch ", "");
            if (args["branch"].as<string>().length() > 0)
            {
                subcommand += args["branch"].as<string>();
            }
        }
        if (args.count("switch"))
        {
            T->switch_ = true;
            Isubcommand(" && git switch ", args["switch"].as<string>());
        }
        if (args.count("delete"))
        {
            T->deleteBranch = true;
            Isubcommand(" && git branch -D ", args["delete"].as<string>());
        }
        if (args.count("Merge"))
        {
            T->merge = true;
            Isubcommand(" && git merge ", args["Merge"].as<string>());
        }
        if (args.count("Pull"))
        {
            T->pull = true;
            if (args["Pull"].as<string>() == "all")
            {
                cout << "Pulling all remote branches..\n";
                Isubcommand(" && git push --all ", "");
            }
            else
            {
                Isubcommand(" && git pull ", args["Pull"].as<string>());
            }
        }
        if (args.count("push"))
        {
            T->push = true;
            if (arg("push", string) == "all")
            {
                cout << "Pushing all branches...\n";
                Isubcommand(" && git push --all ", "");
            }
            else
            {
                Isubcommand(" && git push ", args["push"].as<string>());
            }
        }
        if (args.count("Clone"))
        {
            string tempstr{}, dir{};
            tempstr = arg("Clone", vector<string>).front();
            size_t tsize = arg("Clone", vector<string>).size();
            if (tsize == 2)
            {
                dir = arg("Clone", vector<string>).back();
            }
            else if (tsize > 2)
            {
                errorT2("--Clone / -C takes in only 2 args max, The folder to "
                        "clone "
                        "and the dir to clone into");
            }

            if (tempstr.find("https://github.com") != string::npos || tempstr.find("git@github") != string::npos)
            {
                tempstr += " ";
                tempstr += dir;
                Isubcommand(" && git clone ", tempstr);
                Isubcommand(" ", "");
            }
            else
            {
                string clonestr{"&& git clone "};
                if (args["Request"].as<string>() == "ssh")
                {
                    cout << program_invocation_name << ": using : ssh...\n";
                    clonestr += "git@github.com:";
                    clonestr += tempstr;
                    clonestr += ".git ";
                }
                else
                {
                    cout << program_invocation_name << ": using : https...\n";
                    clonestr += "https://github.com/";
                    clonestr += tempstr;
                    clonestr += " ";
                }
                clonestr += dir;
                Isubcommand(clonestr, " ");
            }
        }
        if (args.count("verbose"))
        {
            T->verbose = true;
        }
        if (args.count("log"))
        {
            T->log = true;
            Isubcommand(" && git log ", "");
        }
        if (args.count("Status"))
        {
            T->status = true;
            Isubcommand(" && git status ", "");
        }
        if (args.count("origin"))
        {
            size_t buffersize = 256;
            char *otemp = new (nothrow) char[buffersize]{};
            T->origin = true;
            snprintf(otemp, buffersize,
                     " && git remote add origin git@github.com:%s && git branch -M "
                     "main && git push -u origin main",
                     arg("origin", string).c_str());
            Isubcommand(otemp, "");
        }
        if (args.count("repo"))
        {
            T->repoName = true;
        }
        if (args.count("Des"))
        {
            T->repoDes = true;
        }
        if (args.count("type"))
        {
            T->repoMode = true;
        }

        if (args.count("undo"))
        {
            T->Rreset = true;
            int trip = int(0);
            string Sreset_temp;
            for (auto str : arg("undo", vector<string>))
            {
                if (str == "soft" || str == "s")
                {
                    if (trip == 0)
                    {
                        Sreset_temp += "--soft HEAD~";
                        trip += 1;
                    }
                }
                else if (str == "hard" || str == "h")
                {
                    if (trip == 0)
                    {
                        Sreset_temp += "--hard HEAD~";
                        trip += 1;
                    }
                }
                else if (str == "mixed" || str == "m")
                {
                    if (trip == 0)
                    {
                        Sreset_temp += "--mixed HEAD~";
                        trip += 1;
                    }
                }
                else if (atoi(str.c_str()) != 0)
                {
                    if (Sreset_temp.length() > 0)
                        Sreset_temp += str;
                }
            }
            if (Sreset_temp.length() <= 0)
                errorT2("Invalid parameters passed to --undo options are [ type of "
                        "reset (hard/mixed/soft) ] [ number of commits back ] \n");

            Resetcommand += " && git reset ";
            Resetcommand += Sreset_temp;
            Isubcommand(Resetcommand, "");
        }
        if (args.count("git"))
        {
            string Git_temp;
            for (auto s : args["git"].as<vector<string>>())
            {
                Git_temp += " && git ";
                Git_temp += s;
            }
            Isubcommand(Git_temp, "");
        }
        if (args.count("Grab"))
        {
            bool lcheck = getdir("/bin", "svn");
            if (!lcheck)
                errorT2("Could not find dependency : Svn... Terminating");
            string grabstr("svn export ");
            grabstr.append(arg("Grab", string));
            int initLenght = grabstr.length();
            // first checks for "tree/main"
            boost::algorithm::replace_all(grabstr, "tree/main", "trunk");
            // if not found, then your most likely exporting a branch
            boost::algorithm::replace_all(grabstr, "tree", "branches");
            if (grabstr.length() == initLenght)
                errorT2("could not resolve github link used\n");
            Isubcommand(" && ", grabstr);
        }
    }
    catch (const opt::error &ex)
    {
        cerr << program_invocation_name << ": " << ex.what() << "\n";
        exit(1);
    }
    try
    {
        parse(T);
    }
    catch (...)
    {
        cout << program_invocation_name << ": Parsing options failed..." << endl;
        exit(1);
    }
    exit(0);
}
/*===== End of Main =======*/

auto Isubcommand(const string_view &s1, const string_view &s2) -> void
{
    subcommand += s1;
    if (s2.length() != 0)
        subcommand += s2;
}

auto errorT2(const string_view &e) -> void
{
    cerr << program_invocation_name << ": " << e;
    exit(1);
}

auto errorT1(const string_view &e) -> void
{
    cerr << program_invocation_name << ": " << e << "\n try \" " << program_invocation_name
         << " --help \" for more information " << endl;
    exit(1);
}

auto getdir(const string &place, const string_view &thing) -> bool
{
    DIR *d;
    struct dirent *dir;
    d = opendir(place.c_str());
    if (d)
    {
        while ((dir = readdir(d)) != null)
        {
            string name = dir->d_name;
            if (name == thing)
            {
                closedir(d);
                return 1;
            }
        }
    }
    return 0;
}

auto getGitInfo() -> bool
{
    FILE *fd;
    int buffersize = 1024;
    string newTemp;
    char *temp = new (nothrow) char[buffersize];
    bool tempTrip{true};
    /* apparently fopen will bug out if there is and error hence 2>&1 is
        needed */
    fd = popen("git status 2>&1 ", "r");
    if (fd == null || temp == null)
        errorT2("Program Crashed...\n");
    /* compares the whole string and if it fails to find the string , it will
     * exit and set tempTrip to true hence a second check will be made */
    while (fgets(temp, buffersize, fd))
    {
        newTemp = temp;
        if (newTemp == "fatal: not a git repository (or any of the parent "
                       "directories): .git\n")
        {
            return !tempTrip;
        }
    }
    /* second check lol */
    fscanf(fd, " %1024s", temp);
    newTemp = temp;
    if (newTemp == "fatal:")
        return !tempTrip;
    return tempTrip;
}

/* this still has issues
 * rets true if there is an add, false if not
 * this should work lmao
 * */
auto Checkadd() -> bool
{
    /* when there is no add we get the strings pasted below */
    FILE *fd = popen(" git status ", "r");
    char *temp = new (nothrow) char[1024];
    if (fd == null || temp == null)
        errorT2("Program Crahsed...\n");
    while (fgets(temp, 1023, fd))
    {
        string_view tempcmp{temp};
        if (tempcmp == "Changes not staged for commit:\n" || tempcmp == "No commits yet\n" ||
            tempcmp == "nothing added to commit but untracked files present (use "
                       "\"git add\" to track)\n" ||
            tempcmp == "Changes to be committed\n")
        {
            /* if string match is found we return false cus we are checking for
             * no add
             */
            fclose(fd);
            return false;
        }
    };
    fclose(fd);
    return true;
}

auto createOnlineRepo() -> void
{
    /* variables we need */
    string token;
    char *crepo = new (nothrow) char[500];

    if (crepo == null)
        errorT2("Program Crashed...\n");
    char confirm;
    /*checking if the file to read the github token from is alive */
    int check = getdir("/usr", "githubToken");
    if (!check)
    {
        errorT2("githubToken file not found\ncreate the file 'githubToken' in /usr "
                "and put only your token inside\nthis is needed to create the "
                "online repository\n");
        exit(1);
    }

    /* getting the token */
    ifstream tokenfile;
    tokenfile.open("/usr/githubToken");
    if (!tokenfile)
    {
        errorT2("Could not read Github token\nit is read from "
                "/usr/githubToken\nplease "
                "create the file and put only the token in...\n");
    }
    tokenfile >> token;
    tokenfile.close();
    /* checking if the token's length is 40 like every other */
    if (token.length() > 41)
        errorT2("Was expecting a Token of char[40]\nFix: remove any whitespace in "
                "/usr/githubToken\n");
    /*converting bool mode to strings */
    /* consent lol */
    cout << " You are about to create an online repository with the following "
            "data :\n"
            "\tName of repository : "
         << reponame
         << "\n"
            "\tDescription : "
         << repodes
         << "\n"
            "\tMode: "
         << (mode == true ? "Private" : "Public")
         << "\n"
            "continue with creation of the online repository [y,N] : ";
    cin >> confirm;
    if ((char)tolower(confirm) != 'y')
        errorT2("User has stopped the process\n");
    check = getdir("/bin", "curl");
    if (!check)
        errorT2("curl not found\nProgram curl is needed for this "
                "operation\nDownload curl to use this feature\n");
    snprintf(crepo, 500,
             "curl -X POST -H \"Authorization: Bearer %s\""
             " https://api.github.com/user/repos -d "
             "'{\"name\":\"%s\",\"description\":\"%s\",\"homepage\":\"https:"
             "//github.com\",\"private\":%s}'",
             token.c_str(), reponame.c_str(), repodes.c_str(), (mode == true ? "true" : "false"));
    /* running it here instead of passing it to run */
    FILE *instance = popen(crepo, "r");
    char buffer[100];
    while (fgets(buffer, 100, instance) != null)
    {
        if (strstr(buffer, "Bad credentials") != null)
        {
            errorT2("Bad credentials : Please update your token, it may have "
                    "expired\n");
        }
    }
    cout << program_invocation_name << ": " << reponame << " repository created succesfully\n";
}

auto run(bool v) -> void
{
    if (subcommand.length())
        command += subcommand;
    if (v)
    {
        cout << "Gitalias VERSION 2.5.0\nCommand generated: " << command << endl;
    }
    system(command.c_str());
    exit(0);
}

auto parse(Trips *t) -> void
{
    string parseCommand;
    bool isInit = getGitInfo();
    /* only when its needed any not for everything */
    /* might remove this in the future */
    if ((t->commit || t->add || t->branch || t->deleteBranch || t->init || t->log || t->switch_ || t->merge ||
         t->status || t->push || t->pull) == true)
    {
        char chrInit;
        if (isInit == 0)
        {
            if (t->init)
            {
                parseCommand += " && git init ";
            }
            else
            {
                cout << "Git repository not found\nDo you want to initialize a "
                        "git "
                        "repository here?[y,N]: ";
                cin >> chrInit;
                if ((char)tolower(chrInit) != 'y')
                {
                    t->init = false;
                    errorT2("No git repository found, exiting..\n");
                    exit(1);
                }
                else
                    parseCommand += " && git init ";
            }
        }
        else
        {
            if (t->init)
                cout << "Git repo already exits skipping...\n";
            t->init = false;
        }
    }
    /* === User repo creation ===*/

    if (t->repoMode && t->repoName && t->repoDes)
    {
        createOnlineRepo();
    }
    else if (t->repoMode || t->repoDes || t->repoName)
    {
        cerr << program_invocation_name
             << ": User cannot create online repository, not enough "
                "information\n"
             << "Must fill fields --repo <string> , --Des <string> and --type "
                "<bool> \n";
    }

    /*if a commit trip is made all these rules apply */
    if (t->commit)
    {
        /*check g trip */
        if (strstr(subcommand.c_str(), "commit") != null)
            errorT2("Err: used commit in --git and --commit together");
        /* if not message trip  */
        if (!t->message)
        {
            messagebox = " -m 'made some changes' ";
        }
        /* if not add trip */
        if (!t->add)
        {
            /* if no added files */
            if (!Checkadd())
                addbox += " && git add . "; // add all
        }

        /* commit trip  occured */
        parseCommand += addbox;
        parseCommand += " && git commit ";
        parseCommand += messagebox;
    }
    else
    {
        if (t->message)
            errorT2("Cannot use option --message without a commit\n");
        if (t->add)
            parseCommand += addbox;
    }
    if (parseCommand.length() > 0)
        command += parseCommand;

    run(t->verbose);
}