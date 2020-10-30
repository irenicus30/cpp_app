import sqlite3
import sys

if __name__ == "__main__":
    database_name = 'example.db'
    if len(sys.argv)>1:
        database_name = sys.argv[1]

    print('database name: ', database_name)
    conn = sqlite3.connect(database_name)

    c = conn.cursor()

    # Create table
    c.execute('''CREATE TABLE users
                 (email text, nickname text, is_active integer, password text)''')

    c.execute("INSERT INTO users VALUES ('email_one','nickname_one',0,'secret_one')")
    c.execute("INSERT INTO users VALUES ('email_two','nickname_two',1,'secret_two')")

    # Save (commit) the changes
    conn.commit()

    conn.close()

        