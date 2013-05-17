#pragma once

template <typename Source, typename Predicate>
class where_enumerator
{
public:
	typedef typename Source::value_type value_type;

private:
	Source source;
	Predicate& predicate;
	
public:
	where_enumerator(Source&& source, Predicate& predicate)
		: source(std::move(source))
		, predicate(predicate)
	{
	}
	
	bool move_next()
	{
		while (true)
		{
			if (!source.move_next())
			{
				return false;
			}
			else if (predicate(source.current()))
			{
				return true;
			}
		}
	}
	
	const value_type& current() const { return source.current(); }
	value_type& current() { return source.current(); }
};