#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <cstring>

#include <BigInt/BigIntegerLibrary.hh>

void totient(BigUnsigned p, BigUnsigned q, BigUnsigned& t) {
	t = (p - 1) * (q - 1);
}

void random_big_int(BigUnsigned& x, int bit_count) {
	for (int i = 0; i < bit_count; i++) {
		BigUnsigned bit = rand() % 2;
		x |= (bit << i);
	}
}

bool miller_test(BigUnsigned d, BigUnsigned n) {
	BigUnsigned a = BigUnsigned(2) + BigUnsigned(rand()) % (n - 4);
	BigUnsigned x = modexp(a, d, n);

	if (x == 1 || x == n-1)
		return true;

	while (d != n-1) {
		x = (x * x) % n;
		d *= 2;
		
		if (x == 1) return false;
		if (x == n-1) return true;
	}

	return false;
}

bool is_prime(BigUnsigned n, BigUnsigned k)
{
	if (n <= 1 || n == 4) return false;
	if (n <= 3) return true;

	BigUnsigned d = n - 1;
	while (d % 2 == 0)
		d /= 2;
	
	for (BigUnsigned i = 0; i < k; i++) {
		if (!miller_test(d, n))
			return false;
	}

	return true;
}

void string_to_num(const std::string& msg, BigUnsigned& num)
{
	for (int i = 0; i < msg.length(); i++) {
		char c = msg.at(i);
		num |= (BigUnsigned(int(c)) << (i * 8));
	}
}

void num_to_string(const BigUnsigned& num, std::string& msg)
{
	for (int i = 0; i < num.bitLength(); i += 8) {
		BigUnsigned c = (num >> i) & BigUnsigned(0xFF);
		msg += char(c.toShort());
	}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Too few arguments!" << std::endl;
		return 1;
	}

	std::cout << "Jonah's Tiny RSA" << std::endl;

	if (!strcmp(argv[1], "gen")) {
		int bit_size = 0;
		std::cout << "Bit Size (512, 1024, 2048, 4096): ";
		std::cin >> bit_size;

		if (bit_size != 512 && bit_size != 1024 && bit_size != 2048 && bit_size != 4096) {
			std::cout << "Please enter a valid bit size for encryption" << std::endl;
			return 1;
		}
		
		std::cout << "Generating P..." << std::endl;
		BigUnsigned p;
		int p_size = bit_size/2;
		random_big_int(p, p_size);
		p.setBit(p_size - 1, true);
		p.setBit(p_size - 2, true);
		p.setBit(0, true);
		while (!is_prime(p, 1))
			p += 2;
		std::cout << "Generated P" << std::endl;
		std::cout << p << std::endl;

		std::cout << "Generating Q..." << std::endl;
		BigUnsigned q;
		int q_size = bit_size - (bit_size / 2);
		random_big_int(q, q_size);
		q.setBit(q_size - 1, true);
		q.setBit(p_size - 2, true);
		q.setBit(0, true);
		while (!is_prime(q, 1))
			q += 2;
		std::cout << "Generated Q" << std::endl;
		std::cout << q << std::endl << std::endl;

		BigUnsigned n = p * q;
		std::cout << "Modulo:" << std::endl;
		std::cout << n << std::endl << std::endl;

		BigUnsigned t;
		totient(p, q, t);
		BigUnsigned e = 65537;
		BigUnsigned d = modinv(e, t);

		std::ofstream pub("rsa.pub");
		pub << n << std::endl;
		pub << e << std::endl;
		pub.close();

		std::ofstream priv("rsa");
		priv << n << std::endl;
		priv << d << std::endl;
		priv.close();

		std::cout << "Public Exponent:" << std::endl;
		std::cout << e << std::endl << std::endl;

		std::cout << "Private Exponent:" << std::endl;
		std::cout << d << std::endl;
	}

	else if (!strcmp(argv[1], "encrypt")) {
		std::string message;
		std::cout << "Message: ";
		std::getline(std::cin, message);

		BigUnsigned num;
		string_to_num(message, num);

		std::string n_str, e_str;
		std::ifstream pub("rsa.pub");
		pub >> n_str >> e_str;
		pub.close();

		BigUnsigned n = BigUnsignedInABase(n_str, 10);
		BigUnsigned e = BigUnsignedInABase(e_str, 10);
		BigUnsigned c = modexp(num, e, n);

		std::cout << std::endl << c << std::endl;
	}

	else if (!strcmp(argv[1], "decrypt")) {
		std::string c_str;
		std::cout << "Encrypted Message: ";
		std::cin >> c_str;

		BigUnsigned c = BigUnsignedInABase(c_str, 10);

		std::string n_str, d_str;
		std::ifstream priv("rsa");
		priv >> n_str >> d_str;
		priv.close();

		BigUnsigned n = BigUnsignedInABase(n_str, 10);
		BigUnsigned d = BigUnsignedInABase(d_str, 10);
		BigUnsigned m = modexp(c, d, n);

		std::string message;
		num_to_string(m, message);

		std::cout << std::endl << message << std::endl;
	}

	else {
		std::cout << "Unknown argument: " << argv[1] << std::endl;
	}

	return 0;
}