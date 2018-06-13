#!/usr/bin/env ruby

require 'csv'
require './stemmify'

# Name,#,Message-ID,Date,From,To,Subject,X-From,X-To,X-cc,X-bcc,X-Folder,X-Origin,X-FileName,content,user,
# From = 4
# To = 5
# Subject = 6
# content = 14
# TF-IDF Ruby adaption inspired by https://github.com/mathieuripert/ruby-tf-idf

STOP_WORDS = [
  'a','cannot','into','our','thus','about','co','is','ours','to','above',
  'could','it','ourselves','together','across','down','its','out','too',
  'after','during','itself','over','toward','afterwards','each','last','own',
  'towards','again','eg','latter','per','under','against','either','latterly',
  'perhaps','until','all','else','least','rather','up','almost','elsewhere',
  'less','same','upon','alone','enough','ltd','seem','us','along','etc',
  'many','seemed','very','already','even','may','seeming','via','also','ever',
  'me','seems','was','although','every','meanwhile','several','we','always',
  'everyone','might','she','well','among','everything','more','should','were',
  'amongst','everywhere','moreover','since','what','an','except','most','so',
  'whatever','and','few','mostly','some','when','another','first','much',
  'somehow','whence','any','for','must','someone','whenever','anyhow',
  'former','my','something','where','anyone','formerly','myself','sometime',
  'whereafter','anything','from','namely','sometimes','whereas','anywhere',
  'further','neither','somewhere','whereby','are','had','never','still',
  'wherein','around','has','nevertheless','such','whereupon','as','have',
  'next','than','wherever','at','he','no','that','whether','be','hence',
  'nobody','the','whither','became','her','none','their','which','because',
  'here','noone','them','while','become','hereafter','nor','themselves','who',
  'becomes','hereby','not','then','whoever','becoming','herein','nothing',
  'thence','whole','been','hereupon','now','there','whom','before','hers',
  'nowhere','thereafter','whose','beforehand','herself','of','thereby','why',
  'behind','him','off','therefore','will','being','himself','often','therein',
  'with','below','his','on','thereupon','within','beside','how','once',
  'these','without','besides','however','one','they','would','between','i',
  'only','this','yet','beyond','ie','onto','those','you','both','if','or',
  'though','your','but','in','other','through','yours','by','inc','others',
  'throughout','yourself','can','indeed','otherwise','thru','yourselves'
]



$file_name = ARGV.length > 0 ? ARGV[0] : 'dataset5k'
$limit = ARGV.length > 1 ? ARGV[1].to_i : 5000
$disp_limit = 100
$train_samples = []
$test_samples = []
$idf = {}
$tfidf_totals = Hash.new(0)
$author_ids = {}
$last_id = 0

puts "Reading data..."
$train_data = CSV.read("processed_data/raw_dataset.csv")
$test_data = CSV.read("processed_data/raw_test_dataset.csv")
$num_samples = $train_data.length + $test_data.length

# compute TF and IDF
def compute_tf_and_idf(data, samples)
  iter = 1

  data.each do |row|
    next unless row[14]

    sample = {
      id: row[1].to_i,
      author: row[0]
    }

    unless $author_ids.has_key?(row[0])
      $last_id += 1
      $author_ids[row[0]] = $last_id
    end

    print "#{iter} TF and IDF: ##{sample[:id]} #{sample[:author]}: "
    text = row[14].stem
    words = text.downcase.gsub(/,|\.|\'|\"/, '').gsub(/[^a-zA-Z]+/, ' ').split(/\s+/)
    num_words = words.length
    print "#{num_words} words\n"

    tf = words.inject(Hash.new(0)) do |h, w|
      unless w.length < 2 || STOP_WORDS.include?(w)
        h[w] += 1.0 / num_words
      end

      h
    end

    tf.keys.each do |w|
      if $idf.has_key?(w)
        y = $num_samples / (10**$idf[w]) + 1
        $idf[w] = Math.log10($num_samples / y)
      else
        $idf[w] = Math.log10($num_samples)
      end
    end

    sample[:tf] = tf
    sample[:num_words] = num_words
    sample[:text] = text
    samples.push sample
    iter += 1
  end
end

# compute and sum TF-IDF
def compute_tf_idf(samples)
  iter = 1

  samples.each do |s|
    tfidf = Hash.new(0)
    puts "#{iter} TF-IDF: ##{s[:id]} #{s[:author]}"

    s[:tf].each do |word, tf|
      tfidf[word] = tf * $idf[word]
      $tfidf_totals[word] += tfidf[word]
    end

    s[:tfidf] = tfidf
    iter += 1
  end
end

def write_samples(samples, file)
  iter = 1

  samples.each do |s|
    puts "Writing sample #{iter}"
    str = "#{$author_ids[s[:author]]},"

    $tfidf_words.each do |arr|
      word = arr[0]

      if s[:tfidf].include? word
        str += s[:tfidf][word].to_s + ","
      else
        str += "0,"
      end
    end

    str.chomp!(',')
    file.puts(str)
    iter += 1
  end
end

puts "TF and IDF: TRAINING DATA"
compute_tf_and_idf($train_data, $train_samples)
puts "TF and IDF: TEST DATA"
compute_tf_and_idf($test_data, $test_samples)
puts "TF-IDF: TRAINING DATA"
compute_tf_idf($train_samples)
puts "TF-IDF: TEST DATA"
compute_tf_idf($test_samples)

# limit to top X words by total TF-IDF
$tfidf_words = $tfidf_totals.sort_by { |h, v| -v }[0..$limit-1]

# output to files
puts "Writing samples: TRAINING DATA"
$file = File.new("processed_data/#{$file_name}.csv", "w")
write_samples($train_samples, $file)
$file.close

puts "Writing samples: TEST DATA"
$file = File.new("processed_data/#{$file_name}_test.csv", "w")
write_samples($test_samples, $file)
$file.close

puts "Writing author IDs to file"

file = File.open("processed_data/author_ids.csv", "w") do |f|
  $author_ids.each { |author, id| f.puts "#{id},#{author}" }
end

puts
puts "Top #{$disp_limit} words"

(1..$disp_limit).each do |i|
  arr = $tfidf_words[i-1]
  puts "#{i}: #{arr[0]}  #{arr[1]}"
end
