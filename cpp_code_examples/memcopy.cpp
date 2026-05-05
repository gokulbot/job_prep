	//void* is generic pointer to cover all the types in c
	void* my_memcpy(void* dest, void* src, size_t n){

		//why unsigned char ??
		//is char even signed ??
		//unsigned char to cover all byte level work??
		//size_t is unit_32 right??
		unsigned char* d = (unsigned char*) dest; // non const unsigned byte pointer to dest 
		const unsigned char* s = (const unsigned char*) src; // const signed byte pointer to src 
		
		d+=n-1;
		s+=n-1;
		
		for(size_t i=n;i>0;i--){
			*d = *s;
			d--;
			s--;
		}
		return dest;
	}
