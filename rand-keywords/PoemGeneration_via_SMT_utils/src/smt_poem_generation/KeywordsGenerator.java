package smt_poem_generation;

import static dio.EasyIO.*;

import java.io.*;
import java.util.*;

public class KeywordsGenerator {
	
	String FILE_ENCODE = "utf-8";
	int randomSeed = 7;
	
//	public void getUniqKeywords(String infile, String outfile)
//	{
//		try{
//			BufferedReader br = getbr(infile);
//			String line = null;
//			LinkedHashSet<String> keywords = new LinkedHashSet<String>();
//			while((line=br.readLine()) != null)
//			{
//				keywords.add(line.trim());
//			}
//			br.close();
//			
//			PrintWriter pw = getpw(outfile);
//			for(String keyword : keywords)
//				pw.println(keyword);
//			pw.close();
//		}catch(Exception e)
//		{
//			e.printStackTrace();
//		}
//	}
//	
//	public void getKeywordsLength(String infile)
//	{
//		try{
//			BufferedReader br = getbr(infile);
//			String line = null;
//			int maxLen = 0;
//			while((line=br.readLine()) != null)
//			{
//				if(line.length() > maxLen)
//					maxLen = line.length();
//			}
//			br.close();
//			println("max keywords length = " + maxLen);
//			int []freqs = new int[maxLen+1];
//			br = getbr(infile);
//			ArrayList<ArrayList<String>> samples = new ArrayList<ArrayList<String>>();
//			int i;
//			for(i = 0; i <= maxLen; i ++)
//				samples.add(new ArrayList<String>());
//			while((line=br.readLine()) != null)
//			{
//				if(line.length() > maxLen)
//					maxLen = line.length();
//				freqs[line.length()] ++;
//				samples.get(line.length()).add(line);
//			}
//			br.close();
//
//			int sum = 0;
//			for(i = 1; i < freqs.length; i ++)
//				sum += freqs[i];
//			println("total frequency = " + sum);
//			for(i = 1; i < freqs.length; i ++)
//			{
//				println("length = " + i + ", freq = " + freqs[i] + ", ratio = " + (double)freqs[i]/sum);
//				ArrayList<String> sample  = samples.get(i);
//				print("Examples: ");
//				for(int j = 0; j < 10 && j < sample.size(); j ++)
//					print(sample.get(j) + " ");
//				println();
//			}
//		}catch(Exception e)
//		{
//			e.printStackTrace();
//		}
//	}
	
	class KeyInfo
	{
		String keyword;
		int freq;
		int accCount;
	}
	int getAcIndex(ArrayList<Integer> acCounts, int val)
	{
		int i;
		for(i = 0; i < acCounts.size(); i ++)
		{
			int e = acCounts.get(i);
			if(val < e)
				break;
		}
		return i;
	}
	public ArrayList<String> generateKeywords(String sxhyF, int N_set, int N_kwsEachSet, boolean withOverlap)
	{
		ArrayList<String> selected = new ArrayList<String>();
		HashSet<String> selectedSet = new HashSet<String>();
		LinkedHashMap<String,Integer> kwsFreq = getKeywordsFreq(sxhyF);
		ArrayList<String> keywordsList = new ArrayList<String>();
		LinkedHashMap<String,KeyInfo> kwsInfo = new LinkedHashMap<String,KeyInfo>();
		ArrayList<Integer> acCounts = new ArrayList<Integer>();
		int sum = 0;
		for(Map.Entry<String, Integer> entry : kwsFreq.entrySet())
		{
			sum += entry.getValue();
			KeyInfo kinfo = new KeyInfo();
			kinfo.keyword = entry.getKey();
			kinfo.accCount = sum;
			kinfo.freq = entry.getValue();
			keywordsList.add(entry.getKey());
			acCounts.add(kinfo.accCount);
		}
		int []keyIdxs = new int[keywordsList.size()];
		int i, j;
		Random rand = new Random(this.randomSeed);
		for(i = 0; i < keyIdxs.length; i ++)
			keyIdxs[i] = i;
		
		boolean []mark = new boolean[keywordsList.size()];
		for(i = 0; i < N_set; i ++)
		{
			if(withOverlap)
			{
				for(j = 0; j < mark.length; j ++)
					mark[j] = false;
			}
			String sKeyword = "";
			for(j = 0; j < N_kwsEachSet; j ++)
			{
				int selectedIndex = -1;
				do
				{
					int val = rand.nextInt(sum);
//					println(sum);
//					println(val);
//					println(acCounts);
					selectedIndex = getAcIndex(acCounts, val);
//					println("selected " + selectedIndex);
				}
				while(mark[selectedIndex]);
				mark[selectedIndex] = true;
				sKeyword += keywordsList.get(selectedIndex) + " ";
			}
			sKeyword = sKeyword.trim();
			if(selectedSet.contains(sKeyword))
				i --;
			else
				selectedSet.add(sKeyword);
		}
		for(String select : selectedSet)
			selected.add(select);
		
		return selected;
	}
	
	LinkedHashMap<String,Integer> getKeywordsFreq(String sxhyF)
	{
		LinkedHashMap<String,Integer> kwsFreq = new LinkedHashMap<String,Integer>();
		try{
			BufferedReader br = getbr(sxhyF, this.FILE_ENCODE);
			String line = null;
			while((line=br.readLine()) != null)
			{
				if(line.startsWith("<")) continue;
				String []fields = line.trim().split("\t");
				if(fields.length != 3) continue;
				String keyword = fields[1].trim();
				String phrStr = fields[2].trim();
				String []phrases = phrStr.split(" ");
				kwsFreq.put(keyword, phrases.length);
			}
			br.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		return kwsFreq;
	}
	
	public ArrayList<String> loadKeywords(String kwF, int N_set, int N_kwsEachSet)
	{
		ArrayList<String> selected = new ArrayList<String>();
		String []keywords = readAllC(kwF,this.FILE_ENCODE).trim().split("\n");
		int i, j;
		int curSize = keywords.length;
		Random rand = new Random(this.randomSeed);
		for(i = 0; i < N_set; i ++)
		{
			String kwset = "";
			for(j = 0; j < N_kwsEachSet; j ++)
			{
				int idx = rand.nextInt(curSize);
				kwset += keywords[idx] + " ";
				if(idx != curSize - 1)
				{
					String tmp = keywords[idx];
					keywords[idx] = keywords[curSize-1];
					keywords[curSize-1] = tmp;
				}
				curSize --;
				if(curSize == 0)
					break;
			}
			if(j == N_kwsEachSet)
				selected.add(kwset.trim());
		}
		return selected;
	}
	public void save(ArrayList<String> selected, String outfile)
	{
		String outstr = join("\n", selected) + "\n";
		write2f(outfile, outstr, this.FILE_ENCODE);
	}
	
	public static void cmdLine(String args[])
	{
		if(args.length != 5)
		{
			println("keywords_generator <keywordsF/SxhyF> <N_set> <N_kwsEachSet> <outF> <withOverlap>");
			return;
		}
		KeywordsGenerator kg = new KeywordsGenerator();
		String keywordsF = args[0];
		int N_set = Integer.parseInt(args[1]);
		int N_kwsEachSet = Integer.parseInt(args[2]);
		String outF = args[3];
		boolean withOverlap = Boolean.parseBoolean(args[4]);
//		ArrayList<String> selected = kg.loadKeywords(keywordsF, N_set, N_kwsEachSet);
		String sxhyF = keywordsF;
		ArrayList<String> selected = kg.generateKeywords(sxhyF, N_set, N_kwsEachSet, withOverlap);
		println(selected);
		kg.save(selected, outF);
	}
	public static void main(String args[])
	{
		cmdLine(args);
	}
}
