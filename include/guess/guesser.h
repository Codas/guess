#ifndef GUESS_GUESSER_H_
#define GUESS_GUESSER_H_

#include <string>
#include <vector>

namespace guess {

struct guesser {
  guesser(std::vector<std::string> const& candidates);
  guesser(std::vector<std::pair<std::string, double>> const& candidates);

  std::vector<int> guess(std::string in, int count = 10) const;

private:
  struct match {
    match() = default;
    explicit match(int index) : index(index), cos_sim(0) {}
    bool operator<(match const& o) const { return cos_sim > o.cos_sim; }
    int index;
    double cos_sim;
  };

  void normalize_all_candidates();

  std::vector<match> match_trigrams(std::string& in) const;

  void score_exact_word_matches(std::string& in,
                                std::vector<match>& matches) const;

  static void normalize(std::string& s);

  std::vector<std::pair<std::string, double>> candidates_;
};

}  // namespace guess

#endif  // GUESS_GUESSER_H_