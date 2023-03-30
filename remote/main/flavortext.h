#ifndef FLAVORTEXTS_H
#define FLAVORTEXTS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


unsigned int linesGenerated;

// The "ing"s are added by the goofy string builder

int random_in_range(int x){
	//return rand() % x;
	return esp_random() % x;
}

const char constructiveVerbs[][11] = {
  "Align",
  "Build",
  "Calibrat",
  "Instanc",
  "Configur",
  "Snort",
  "Microwav",
  "Tweak",
  "Wrangl",
  "Hack",

  "Pwn",
  "Boot",
  "Allocat",
  "Bind",
  "Revv",
  "Polish",
  "Fabricat",
  "Ping",
  "Refactor",
  "Load",

  "Quantify",
  "Assembl",
  "Distill",
  "Bak",
  "Receiv",
  "Unlock",
  "Compil",
  "Pressuriz",
  "Chooch",
  "Mak",

  "Engag",
  "Decrypt",
  "Synthesiz",
  "Predict",
  "Analyz",
  "Dispens",
  "Fir",
  "Insert",
  "Align",
  "Encourag",

  "Extrud",
  "Access",
  "Sharpen",
  "Enhanc",
  "Crank",
  "Stack",
  "Craft",
  "Render",
  "Mount",
  "Generat",

  "Implement",
  "Download",
  "Construct",
  "Wow! Amaz",
  "Moisten",
  "Customiz",
  "Compensat",
  "Buffer",
  "Transferr",
  "Induct",

  "Emitt",
  "Unzipp",
  "Squirt",
  "Feed",
  "Buy",
  "Spark",
  "Implant",
  "Triangulat",
  "Inject",
  "Link",
  "Brew",

  "Process",
  "Deploy",
  "Tun",
  "Attach",
  "Train",
  "Ignor",
  "Tapp",
  "Reload",
  "Simulat",
  "Fluff",

  "Fill",
  "Sort",
  "Updat",
  "Upgrad",
  "Prim",
  "Trac",
  "Inflat",
  "Wangjangl",
  "Charg",
  "Crack",

  "Ignor",
  "Activat",
  "Dial",
  "Pimp",
  "Collect",
  "Approach",
  "Approv",
  "Sampl",
  "Energiz",
  "Stuff"
};

#define numberOfConstructiveVerbs 100

const char destructiveVerbs[][11] = {
  "Deallocat",
  "Trash",
  "Unplugg",
  "Revok",
  "Forgett",
  "Discard",
  "Dropp",
  "Holster",
  "Shredd",
  "Jettison",

  "Dissolv",
  "Liquidat",
  "Releas",
  "Collimat",
  "Eject",
  "Ditch",
  "Leak",
  "Sell",
  "Banish",
  "Dereferenc",

  "Sacrific",
  "Desolder",
  "Destruct",
  "Decompil",
  "Blow",
  "Disengag",
  "Digest",
  "Smash",
  "Encrypt",
  "Crash",

  "Lock",
  "Purg",
  "Regrett",
  "Rewind",
  "Free",
  "Delet",
  "Clos",
  "Retract",
  "Collaps",
  "Liquefy",

  "Derezz",
  "Stow",
  "Archiv",
  "Suspend",
  "Suppress",
  "Clean",
  "Squash",
  "Secur",
  "Withdraw",
  "Dump",

  "Obfuscat",
  "Break",
  "Scrubb",
  "Abandon",
  "Flatten",
  "Stash",
  "Finish",
  "Evacuat",
  "Scrambl",
  "Recycl",

  "Crush",
  "Zipp",
  "Unload",
  "Disconnect",
  "Loosen",
  "Contain",
  "Debat",
  "Detach",
  "Neutraliz",
  "Salvag",

  "Empty",
  "Hid",
  "Disarm",
  "Pickl",
  "Disregard",
  "Yeet",
  "Scrapp",
  "Deflat",
  "Discharg",
  "Deactivat",

  "Steriliz",
  "Reliev",
  "Nuk",
  "Degauss",
  "Dismiss",
  "Drain",
  "Reject",
  "Nerf",
  "Pay",
  "Return",

  "Unstick",
  "Splitt",
  "Cancell",
  "Sham",
  "Embezzl",
  "Fling",
  "Regrett",
  "Halt",
  "Arrest",
  "Bury"
};

#define numberOfDestructiveVerbs 100

#define numberOfNouns 100

const char nouns[numberOfNouns][13] = {
  "content",
  "your mom",
  "the shmoo",
  "API",
  "the BJT man",
  "aesthetics",
  "backstory",
  "tactics",
  "bugs",
  "sauce",

  "warp drive",
  "data",
  "the funk",
  "AI",
  "crystals",
  "spaghetti",
  "fluxgate",
  "electrons",
  "loud noises",
  "wires",

  "bytecode",
  "the truth",
  "magic",
  "hot lava",
  "bits",
  "Brad",
  "Teensy",
  "sensors",
  "photons",
  "signal",

  "the planet",
  "password",
  "chips",
  "circuits",
  "privacy",
  "synergy",
  "widgets",
  "love",
  "packets",
  "reality",

  "lasers",
  "protocols",
  "voltage",
  "registers",
  "puns",
  "dogecoins",
  "kittens",
  "magic smoke",
  "plot device",
  "the core",

  "dank memes",
  "subroutines",
  "radiation",
  "steam",
  "trousers",
  "beer",
  "protocol",
  "one-liners",
  "the Gibson",
  "software",

  "a fat one",
  "holograms",
  "magnets",
  "inductors",
  "resistors",
  "capacitors",
  "viewers",
  "subscribers",
  "sausage",
  "my wife",

  "drama",
  "the future",
  "vectors",
  "the clowns",
  "a Palm Pilot",
  "5G implant",
  "monkeys",
  "breadboard",
  "Patreon",
  "money",

  "the Internet",
  "fluids",
  "the impostor",
  "beats",
  "dopamine",
  "fedora",
  "neural net",
  "comments",
  "ports",
  "you. Yes you",

  "mixtape",
  "[REDACTED]",
  "hot tub",
  "paperwork",
  "Nerf",
  "cyber-doobie",
  "the 1%",
  "the Matrix",
  "variables",
  "IP address"
};

void getFullLine(char * buffer, bool constructive) {
  static int lastConstructiveVerbNumber = numberOfConstructiveVerbs;
  static int lastDestructiveVerbNumber = numberOfDestructiveVerbs;
  static int lastNounNumber = numberOfNouns;

  int verbNumber = random_in_range(constructive ? numberOfConstructiveVerbs : numberOfDestructiveVerbs);
  int verbLength = strlen(constructive ? constructiveVerbs[verbNumber] : destructiveVerbs[verbNumber]);
  int nounNumber = random_in_range(numberOfNouns);
  int nounLength = strlen(nouns[nounNumber]);

  while ((constructive ? verbNumber == lastConstructiveVerbNumber : verbNumber == lastDestructiveVerbNumber)
         || nounNumber == lastNounNumber
         || verbLength + nounLength > 16) { // We need 4 extra chars for 'ing' and the space
    verbNumber = random_in_range(constructive ? numberOfConstructiveVerbs : numberOfDestructiveVerbs);
    verbLength = strlen(constructive ? constructiveVerbs[verbNumber] : destructiveVerbs[verbNumber]);
    nounNumber = random_in_range(numberOfNouns);
    nounLength = strlen(nouns[nounNumber]);
  }

  //std::string output = (constructive ? constructiveVerbs[verbNumber] : destructiveVerbs[verbNumber]);
  //output += "ing ";

  //output += nouns[nounNumber];
  //if (output.length() < 18) output += "...";

  //output.toCharArray(buffer, 21);
	//strcpy(buffer, output.c_str());

  if (constructive) lastConstructiveVerbNumber = verbNumber;
  else lastDestructiveVerbNumber = verbNumber;
  lastNounNumber = nounNumber;

	//char buffer[20];	
	memset(buffer, 0, strlen(buffer));
	strcat(buffer, (constructive ? constructiveVerbs[verbNumber] : destructiveVerbs[verbNumber]));
	char ing[] = {'i', 'n', 'g', '\0'};
	strcat(buffer, ing);
	char space[] ={' ', '\0'};
	strcat(buffer, space);
	strcat(buffer, nouns[nounNumber]);

  linesGenerated++;
}

#endif

// You can't spell 'flavortext' without 'vortex'
