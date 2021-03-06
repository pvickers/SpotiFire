#include "stdafx.h"

#include "Artist.h"
#define SP_TYPE(type_name, ptrPtr) (type_name *)(void *)ptrPtr

#define ARTIST_LOADED(ptr) if(!sp_artist_is_loaded(ptr)) throw gcnew NotLoadedException("Artist")

static __forceinline String^ HEX(const byte *bytes, int count)
{
	if(bytes==NULL) return String::Empty;
	char result[41];
	result[40] = '\0';
	char *current = result;
	for(int i = 0; i < count; i++) {
		sprintf(current, "%02X", bytes[i]);
		current += 2;
	}
	return UTF8(result);
}

Artist::Artist(SpotiFire::Session ^session, sp_artist *ptr) {
	SPLock lock;
	_ptr = ptr;
	_session = session;
	sp_artist_add_ref(_ptr);
}

Artist::~Artist() {
	this->!Artist();
}

Artist::!Artist() {
	SPLock lock;
	sp_artist_release(_ptr);
	_ptr = NULL;
}

Session ^Artist::Session::get() {
	return _session;
}

bool Artist::IsReady::get() {
	SPLock lock;
	const char *name = sp_artist_name(_ptr);
	return name != NULL && strlen(name) > 0;
}

bool Artist::IsLoaded::get() {
	SPLock lock;
	return sp_artist_is_loaded(_ptr);
}

String ^Artist::Name::get() {
	SPLock lock;
	ARTIST_LOADED(_ptr);
	return UTF8(sp_artist_name(_ptr));
}

String^ Artist::PortraitId::get() {
	SPLock lock;
	return HEX(sp_artist_portrait(_ptr, SP_IMAGE_SIZE_NORMAL), 20);
}
ArtistBrowse ^Artist::Browse(ArtistBrowseType type) {
	return ArtistBrowse::Create(_session, this, type);
}

Link ^Artist::GetLink() {
	return Link::Create(this);
}

int Artist::GetHashCode() {
	return (new IntPtr(_ptr))->GetHashCode();
}

bool Artist::Equals(Object^ other) {
	return other != nullptr && GetType() == other->GetType() && GetHashCode() == other->GetHashCode();
}

bool Artist::operator== (Artist^ left, Artist^ right) {
	return Object::ReferenceEquals(left, right) || (!Object::ReferenceEquals(left, nullptr) && left->Equals(right));
}

bool Artist::operator!= (Artist^ left, Artist^ right) {
	return !(left == right);
}