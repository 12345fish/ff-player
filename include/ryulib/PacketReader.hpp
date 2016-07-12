#ifndef RYULIB_PACKETREADER_HPP
#define RYULIB_PACKETREADER_HPP

/// PacketReader ���� ������ �޸� ���ġ�� �ʹ� ����ϰ� �߻����� �ʵ��� 
#define PACKETREADER_PAGE_SIZE 32 * 1024

class PacketReader
{
private:
	void *buffer_ = nullptr;
	int buffer_size_ = 0;
	int offset_ = 0;
	int capacity_ = 0;
	char *offset_ptr_ = nullptr;
public:
	PacketReader() 
	{
	}

	~PacketReader() 
	{
		Clear();
	}

	void Clear()
	{
		buffer_size_ = 0;
		offset_ = 0;
		capacity_ = 0;

		if (buffer_ != nullptr) free(buffer_);
		buffer_ = nullptr;

		offset_ptr_ = nullptr;
	}

	/**
		size ũ���� data�� PacketReader ���� ���ۿ� �߰��Ѵ�.
	*/
	void Write(const void *data, const int size)
	{
		if (size <= 0) return;

		int new_size = buffer_size_ + size;

		// ���ο� �����͸� ���� ���� ������ �����ϴ�.
		if ((new_size + offset_) > capacity_) {
			capacity_ = ((new_size / PACKETREADER_PAGE_SIZE) + 1) * PACKETREADER_PAGE_SIZE;

			void *new_data = malloc(capacity_);
			char *temp_index = (char *) new_data;

			if (buffer_size_ > 0) {
				memcpy(temp_index, offset_ptr_, buffer_size_);
				temp_index = temp_index + buffer_size_;
			}

			memcpy(temp_index, data, size);

			offset_ = 0;

			void *old_data = buffer_;
			buffer_ = new_data;

			if (old_data != nullptr) free(old_data);

			offset_ptr_ = (char *)buffer_;
		} else {
			char *temp_index = offset_ptr_ + buffer_size_;
			memcpy(temp_index, data, size);
		}

		buffer_size_ = new_size;
	}

	/**
		size ��ŭ�� �����͸� �о� �´�.  ���ο� ����� �����Ͱ� ������ nullptr�� �����Ѵ�.
	*/
	void *Read(const int size)
	{
		if (buffer_size_ < size) return nullptr;

		char *result = offset_ptr_;

		buffer_size_ = buffer_size_ - size;
		offset_ = offset_ + size;
		offset_ptr_ = offset_ptr_ + size;

		return result;
	}

	/**
		size ũ���� �����͸� ���� �� �ִ°�?  ��, ���ο� �� ũ�� �̻��� �����Ͱ� �׿��°�?
	*/
	bool canRead(const int size)
	{
		if (offset_ptr_ == nullptr) return nullptr;
		else return buffer_size_ >= size;
	}
};

#endif  // RYULIB_PACKETREADER_HPP