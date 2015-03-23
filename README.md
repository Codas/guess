# `guess` Libary

This library is basically a fast fuzzy string matcher based on [trigrams](http://en.wikipedia.org/wiki/Trigram) and [Cosine similarity](http://en.wikipedia.org/wiki/Cosine_similarity).

Example usage:

    std::vector<std::string> names { "Hello", "World" };
    guesser g(names);
    for (auto const& str : g.guess("Hallo")) {
      std::cout << str << "\n";
    }