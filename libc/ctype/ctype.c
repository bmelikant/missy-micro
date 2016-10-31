/*
 * ctype.c
 *
 *  Created on: Oct 30, 2016
 *      Author: bmmelikant
 */

int isalpha(int c) { if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1; else return 0; }
int isdigit(int c) { if (c >= '0' && c <= '9') return 1; else return 0; }
int isalnum(int c) { if (isdigit(c) || isalpha (c)) return 1; else return 0;}
int isspace(int c) { if (c == ' ' || c == '\t') return 1; else return 0;}
int isupper(int c) { if (c >= 'A' && c <= 'Z') return 1; else return 0; }

