The Parser will parse json and callback functions.

You need to pass it an object that has these template functions to call:

template <typename T>
mapper.foundSimpleValue(T);

mapper.foundNull();

mapper.startArray();
mapper.endArray();

mapper.startObj();
mapper.endObj();
mapper.propertyName(string&& name);
