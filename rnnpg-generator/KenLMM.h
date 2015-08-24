/*
 * KenLMM.h
 *
 *  Created on: 8 Mar 2014
 *      Author: s1270921
 */

#ifndef KENLMM_H_
#define KENLMM_H_

#include <vector>
#include <string>
using namespace std;
#include "lm/model.hh"
using namespace lm::ngram;

class KenLMM
{
public:
	KenLMM(Model *_model = NULL) : model(_model) {}
	void setModel(Model* _model) { model = _model; }
	void assignProbSentence(vector<string> words, bool sentenceContext)
	{
		State state, out;
		lm::FullScoreReturn ret;
		state = sentenceContext ? model->BeginSentenceState() : model->NullContextState();
		size_t i;
		double total = 0.0;
		for(i = 0; i < words.size(); i ++)
		{
			lm::WordIndex vocab = model->GetVocabulary().Index(words[i]);
			ret = model->FullScore(state, vocab, out);
			total += ret.prob;
			cout << words[i] << '=' << vocab << ' ' << static_cast<unsigned int>(ret.ngram_length)  << ' ' << ret.prob << '\t';
			state = out;
		}

		if(sentenceContext)
		{
			ret = model->FullScore(state, model->GetVocabulary().EndSentence(), out);
			total += ret.prob;
			cout << "</s>=" << model->GetVocabulary().EndSentence()
					<< ' ' << static_cast<unsigned int>(ret.ngram_length)  << ' ' << ret.prob << '\t';
		}

		cout << "Total : " << total << endl;
	}

	double getProbs(vector<string> &ngram, int startPos, vector<double> &probs)
	{
		assert(ngram.size() > 0);
		probs.clear();
		const double unknownWordLogProb = -100.0;
		double log10Prob = 0, lprob = 0;
		State state, out_state;
		size_t i = 0;
		if(ngram[0] == "</s>" || ngram[0] == "<s>")
		{
			state = model->BeginSentenceState();
			i ++;
		}
		else
			state = model->NullContextState();
		const Vocabulary &vocab = model->GetVocabulary();

		for( ; i < (size_t)startPos; i ++)
		{
			model->Score(state, vocab.Index(ngram[i]), out_state);
			state = out_state;
		}

		for( ; i < ngram.size() && ngram[i] != "</s>"; i ++)
		{
			lprob = model->Score(state, vocab.Index(ngram[i]), out_state);
			probs.push_back(exp10(lprob));
			log10Prob += lprob;
			state = out_state;
		}
		if(i < ngram.size())
		{
			lprob = model->Score(state, vocab.EndSentence(), out_state);
			probs.push_back(exp10(lprob));
			log10Prob += lprob;
		}

		log10Prob != 0 ? log10Prob : unknownWordLogProb;
		return log10Prob * log(10);
	}

	double getLog10Prob(vector<string> &ngram, int startPos = 0)
	{
		assert(ngram.size() > 0);
		const double unknownWordLogProb = -100.0;
		double log10Prob = 0, lprob = 0;
		State state, out_state;
		size_t i = 0;
		if(ngram[0] == "</s>" || ngram[0] == "<s>")
		{
			state = model->BeginSentenceState();
			i ++;
		}
		else
			state = model->NullContextState();
		const Vocabulary &vocab = model->GetVocabulary();

		for( ; i < (size_t)startPos; i ++)
		{
			model->Score(state, vocab.Index(ngram[i]), out_state);
			state = out_state;
		}

		for( ; i < ngram.size() && ngram[i] != "</s>"; i ++)
		{
			lprob = model->Score(state, vocab.Index(ngram[i]), out_state);
			// cout << ngram[i] << " " << lprob << endl;
			log10Prob += lprob;
			state = out_state;
		}
		if(i < ngram.size())
			log10Prob += model->Score(state, vocab.EndSentence(), out_state);

		return log10Prob != 0 ? log10Prob : unknownWordLogProb;
	}

	double getLogeProb(vector<string> &ngram, int startPos = 0)
	{
		return getLog10Prob(ngram, startPos) * log(10);
	}
private:
	Model *model;
};

#endif /* KENLMM_H_ */
