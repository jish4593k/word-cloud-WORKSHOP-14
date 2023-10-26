import sqlite3
from wordcloud import WordCloud
import matplotlib.pyplot as plt
import re
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
import string

# Function to preprocess and clean the text
def clean_text(text):
    # Convert to lowercase
    text = text.lower()
    
    # Remove HTML tags
    text = re.sub(r'<[^>]+>', '', text)
    
    # Tokenize the text
    words = word_tokenize(text)
    
    # Remove punctuation and stopwords
    words = [word for word in words if word not in string.punctuation and word not in stopwords.words('english')]
    
    return ' '.join(words)

# Function to create a word cloud from text
def create_word_cloud(text):
    print('Generating a word cloud from WeChat chat records...')

    # Remove HTML tags, stopwords, and punctuation
    text = clean_text(text)

    # Create a WordCloud
    wc = WordCloud(
        background_color='white',
        max_words=100,
        width=800,
        height=600,
        colormap='viridis'  # Use a different colormap
    )
    wordcloud = wc.generate(text)

    # Save the word cloud image
    wordcloud.to_file("wordcloud.png")

    # Display the word cloud
    plt.figure(figsize=(8, 6))
    plt.imshow(wordcloud, interpolation='bilinear')
    plt.axis("off")
    plt.show()

# Function to extract chat records from the database
def get_content_from_weixin():
    # Establish a database connection
    conn = sqlite3.connect("weixin.db")
    cursor = conn.cursor()

    # Get the names of all chat tables
    table_names = cursor.execute("SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE 'Chat\_%' escape '\\\'").fetchall()

    content = ''

    for table_name in table_names:
        sql = f"SELECT Message FROM {table_name[0]}"
        cursor.execute(sql)
        results = cursor.fetchall()

        for result in results:
            content += str(result)

    conn.commit()
    cursor.close()
    conn.close()

    return content

# Get chat content from the database
content = get_content_from_weixin()

# Generate a word cloud from the chat records
create_word_cloud(content)
