//
//  main.cpp
//  A1
//
//  Created by Marc Roopchand on 11-05-21.
//  Copyright 2011 University of Waterloo. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class PostCode {
public:
	PostCode();							// Construct unset postal code 
	~PostCode();						// Destructor 
	std::string postcode() const;		// Accessor
	bool postcodeIs(string p);		    // Mutator
	bool isUnset() const;			    // Test if postal code is unset
	
private:
	std::string *postcode_;
	PostCode& operator=(const PostCode&);	    // Disable assignment
};


//************************************************************************
//Constructors, Destructors
//************************************************************************

PostCode::PostCode(){
	postcode_ = NULL ;
}

PostCode::~PostCode(){
	delete postcode_ ;
}


//************************************************************************
//Accessors
//************************************************************************

std::string PostCode::postcode() const {
	return *postcode_ ;
}

//************************************************************************
//Mutators
//************************************************************************

bool PostCode::postcodeIs(std::string p) {
	bool check_code  = false ;
	string not_ = "DFIOQU" ;
	
	if(isUnset() && p.length() == 7){
		
		if((p.at(0) > 64 && p.at(0) < 91) && (p.at(2) > 64 && p.at(2) < 91) && (p.at(5) > 64 && p.at(5) < 91)) {	//if 1st, 3rd and 6th chars are capital letters
			
			if((p.at(1) >= 47 && p.at(1) <= 58) && (p.at(4) >= 47 && p.at(4) <= 58) && (p.at(6) >=47  && p.at(6) <= 58)){ //if 2nd, 5th and 7th chars are numbers
				
				if(p.at(3) == 32){							//if 4th char is space
					
					if(p.at(0) != 'W' && p.at(0) != 'Z'){  //first char is not Z or W
						
						if((not_.find(p.at(0)) == -1) && (not_.find(p.at(2)) == -1) && (not_.find(p.at(5))) == -1) {  //checking that that the characters D,F,I,O,Q,U are not present as the 1st, 3rd or 6th
							check_code = true ;
							string* s = new string(p) ;
							postcode_ = s ;
						}
					}
				}
			}
		}
	}
	return check_code ;
}

//************************************************************************
//  Helper functions
//************************************************************************

//checks if post code is set
bool PostCode::isUnset() const {
	if(postcode_)
		return false ;
	else
		return true ;
}


// check whether whether test harness command has a second operand
bool anotherOperand(stringstream &inputSS) {
	char space = inputSS.peek();
	
	if ( inputSS.eof() || space != ' ' ) {
		return false;
	}
	else {
		return true;
	}
}

//************************************************************************
// main()
//************************************************************************

int main(int argc, char* argv[]) {
	
	cout << "Test harness for Postal Code ADT" << endl << endl;
	PostCode *p = new PostCode();
	
	cout << "Command: ";
	
	// read whole input line into string, then "stream" from string
	string input;
	while ( getline(cin, input) ) {		
		stringstream inputSS(input);
		
		// parse command: one letter command followed by a space
		char command;
		inputSS >> command;
		
		// process command
		switch (command) {
				// new postal code object
			case 'n': {
				delete p;
				p = new PostCode();
				break;
			}
				
				// set postal code value of existing object
			case 's': {
				if ( anotherOperand(inputSS) ) {
					string code;
					inputSS >> code;
					
					if ( anotherOperand(inputSS) ) {
						string code2;
						inputSS >> code2;
						code = code + " " + code2;
					}
					
					if ( p->postcodeIs(code) ) {
						cout << "Postal Code has been set." << endl;
					}
					else {
						cout << "Postal Code is either already set, or provided value does not meet postal-code rules." << endl;
					}
				}
				else {
					cout << "Command takes a second operand." << endl;
				}
				
				break;
			}
				
				
				// print postal code
			case 'p': {
				if ( p->isUnset() ) {
					cout << "Postal Code is unset." << endl;
				}
				else {
					cout << "Postal Code value is \"" << p->postcode() << "\"" << endl;
				}
				
				break;
			}
				
				
				
				// check whether postal code is unset
			case 'u': {
				if ( p->isUnset() ) {
					cout << "TRUE. Postal Code is unset." << endl;
				}
				else {
					cout << "FALSE. Postal Code has been set." << endl;
				}
				break;
			}	
				
			default:
				cout << "Invalid command." << endl;
		} // switch
		
		cout << endl << "Command: ";
	} // while input
	
	
}
