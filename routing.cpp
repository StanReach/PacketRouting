/*
 * routing.cpp
 *
 *  Created on: 8 Mar 2018
 *      Author: Mattia
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class RoutingAddr {

private:
	string dest;
	string mask;
	string gate;

public:
	RoutingAddr() {}
	// non modifying operators
	string get_dest() {
		return dest;
	}
	string get_mask() {
		return mask;
	}
	string get_gate() {
		return gate;
	}
	// modifying operators
	void set_dest(string d) {
		dest = d;
	}
	void set_mask(string m) {
		mask = m;
	}
	void set_gate(string g) {
		gate = g;
	}
};

//utility functions
string dec_to_bin(int number) {
    if ( number == 0 ) return "0";
    if ( number == 1 ) return "1";

    if ( number % 2 == 0 )
        return dec_to_bin(number / 2) + "0";
    else
        return dec_to_bin(number / 2) + "1";
}


int bin_to_dec(string number) {
    int result = 0, pow = 1;
    for ( int i = number.length() - 1; i >= 0; --i, pow <<= 1 )
        result += (number[i] - '0') * pow;

    return result;
}


vector<string> remove_points(string str) { // return a vector of string containing the address without dots(in the dotted decimal notation)
	vector<string> point_less(0);
	int last_point_pos = 0;
	int count=0;
	string temp;
	for( size_t i =0; i < str.size(); ++i) {
		if (str[i]=='.') {
		    point_less.resize(count+1);
			for(size_t ii = last_point_pos; ii < i; ++ii) {
				temp += str[ii];
    		}
			last_point_pos = i+1;
    		point_less[count] = temp;
    		temp.erase(0,temp.size());
    		count += 1;
		} else if (i == str.size()-1) {
		    point_less.resize(count+1);
		    for(size_t ii = last_point_pos; ii < str.size(); ++ii) {
				temp += str[ii];
    		}
			last_point_pos = i+1;
    		point_less[count] = temp;
    		count += 1;
		}
	}
    return point_less;
}


vector<int> stoi_vect(vector<string> str) { // return a vector of int by transforming the num strings of address in ints
	vector<int> nums(0);
	int k=0;
	for (size_t j=0; j < str.size(); ++j) {
		k = stoi(str[j]);
		nums.resize(nums.size()+1);
		nums[nums.size()-1] = k;
	}
	return nums;
}


vector<string> bin_vect(vector<int> decs) { // return a binary vector from the vector of int containing int address
    vector<string> bins(0);
    string nums;
	for (size_t k=0; k < decs.size(); ++k) {
		nums = dec_to_bin(decs[k]);
		if(nums.size() < 8) {
			nums = string((8-nums.size()), '0').append(nums);
		}
		bins.resize(bins.size()+1);
		bins[bins.size()-1] = nums;

	}
	return bins;
}


int char_to_int(char s) {
    int result = 0;
    if (s == '1') {
        result = 1;
    } else if ( s == '0') {
        result = 0;
    } else {
    	result = -1;
    }
    return result;
}


char int_to_char(int n) {
    char result = '0';
    if (n == 1) {
        result = '1';
    } else if (n == 0) {
        result = '0';
    } else {
    	result = '0';
    }
    return result;
}


vector<string> bin_anding(vector<string> dest, vector<string> mask) { //returns the ANDed IP destination and table mask
	vector<string> result(0);
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
	string temp4;
	char push;
	for (size_t i =0; i < dest.size(); ++i) {
	    for (size_t ii=0; ii < dest[i].size(); ++ii) {
    		temp1 = char_to_int(dest[i][ii]);
    		temp2 = char_to_int(mask[i][ii]);
    		temp3 = temp1 & temp2;
    		push = int_to_char(temp3);
    		temp4 += push;
    		if (ii == dest[i].size()-1) {
        		result.resize(result.size()+1);
        		result[result.size()-1]=temp4;
        		temp4.erase(0, temp4.size());
    		}
	    }
	}
	return result;
}

// both vector are intend for binary use
bool bin_matcher(vector<string> bin_dest, vector<string> bin_and) { //check if the binary destination on the router table match the ANDed sequence
	bool check;
	for (size_t i=0; i < bin_dest.size(); ++i) {
		if (bin_dest[i]==bin_and[i]) {
			check = true;
		} else {
			check = false;
			return check;
		}
	}
	return check;
}


string find_next_hop(string ip, vector<RoutingAddr> table, int level) { //to be used in a main, looping on router table lines for each IP
	vector<string> rdest_pointless(0);    // employs utility functions to correctly check if there is a match between current ANDed IP and table destination
	for (int i=level-1; i < level; ++i) { // first router destination on table is level = 1
		rdest_pointless = remove_points(table[i].get_dest());
	}

	vector<string> mask_pointless(0);
	for (int i=level-1; i < level; ++i) { // first router destination on table is level = 1
		mask_pointless = remove_points(table[i].get_mask());
	}

	vector<string> rdest_bin = bin_vect(stoi_vect(rdest_pointless));
	vector<string> mask_bin = bin_vect(stoi_vect(mask_pointless));
	vector<string> ip_bin = bin_vect(stoi_vect(remove_points(ip)));

	vector<string> ip_and_mask = bin_anding(ip_bin,mask_bin);
	bool checker = bin_matcher(rdest_bin, ip_and_mask);
	string result;
	if (checker == true) {
		for(int i = level-1; i < level; ++i) {
			result = table[i].get_gate();
		}
	} else {
		result = "false";
	}
	return result;
}

vector<RoutingAddr> sort_table(vector<RoutingAddr> table) { // sort the routing table in decreasing order based on the non-zeros mask length
	vector<RoutingAddr> temp(0);
	int counter=0;
	vector<int> mask_counts(0);
	vector<string> temp2(0);
	vector<int> temp3(0);
	for(size_t i=0; i < table.size(); ++i) {
		for(size_t ii=1; ii < 2; ++ii) {
			temp2 = remove_points(table[i].get_mask());
			temp3 = stoi_vect(temp2);
			for (size_t j=0; j < temp3.size(); ++j) {
				if (temp3[j] != 0) {
					++counter;
				}
			}
			mask_counts.push_back(counter);
			counter = 0;
		}
	}
	for(int a=4; a >= 0; --a) {
	    for(size_t k=0; k < mask_counts.size(); ++k) {
	        if(mask_counts[k] == a) {
	            temp.push_back(table[k]);
	        }
	    }
	}
	return temp;
}

int main() {
	RoutingAddr address;
	vector<RoutingAddr> table(0);
	string dest;
	string mask;
	string gate;
	int addr_num;
	int dest_num;
	string ip;
	string result;
	cin >> addr_num >> dest_num;
	int addr_num2 = addr_num;
	while (addr_num > 0) {
		cin >> dest >> mask >> gate;
		address.set_dest(dest);
		address.set_mask(mask);
		address.set_gate(gate);
		table.push_back(address);
		--addr_num;
	}
	table = sort_table(table);
	int level = 1;
	int addr_num3 = addr_num2;
	while (dest_num > 0) {
		cin >> ip;
		while (addr_num2 > 0) {
    		result = find_next_hop(ip, table, level);
    		if (result == "false") {
    			++level;
    	        --addr_num2;
    		} else {
    			if (dest_num == 1) {
    				cout << result;
    			} else {
    				cout << result << '\n';
    			}
    			level = 1;
    			--dest_num;
    			addr_num2=addr_num3;
    			break;
    		}
		}
	}
}

