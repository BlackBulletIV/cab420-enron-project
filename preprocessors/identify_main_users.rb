#!/usr/bin/env ruby

Dir.chdir('data/enron')
names = Dir.glob('*')
Dir.chdir('../../processed_data/users')
count = 0

folders = {}

Dir.glob('*').each do |u|
  names.each do |n|
    parts = n.split('-')

    if u.start_with?(parts[1]) && u.end_with?(parts[0])
      if folders.key? n
        folders[n].push u
      else
        folders[n] = [u]
      end

      File.rename(u, '../filtered_users/' + u)
      count += 1
      break
    end
  end
end

folders.each do |f, list|
  puts f

  list.each do |u|
    puts u
  end

  puts
end

puts count

