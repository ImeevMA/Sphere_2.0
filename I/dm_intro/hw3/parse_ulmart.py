from bs4 import BeautifulSoup
import urllib.parse as up
import pandas as pd
import requests

urls = {
    "memory":"https://www.ulmart.ru/catalog/memory?pageNum=",
}

items = dict()

for name, url in urls.items():
    tmp_items = list()
    for i in range(1, 8):
        page = requests.get(url + str(i))
        soup = BeautifulSoup(page.text, 'html.parser')
        tmp_items = tmp_items + [(x['data-gtm-eventproductprice'], up.urljoin("https://www.ulmart.ru", x['href']))
            for x in soup.find_all('a', class_="js-gtm-product-click")]
    items[name] = tmp_items

items["memory"] = set(items["memory"])

mem = list()
for price, url in items["memory"]:
    page = requests.get(url)
    soup = BeautifulSoup(page.text, 'html.parser')
    mem.append([int(price[:-3])] + [x.text.strip() for x in soup.find_all(class_="b-dotted-line__content")])

mem1 = list()
for x in mem:
    n = 1
    if(x[1] == 'Комплект из двух модулей'):
        n = 2
    elif(x[1] == 'Комплект из четырех модулей'):
        n = 4
    mem1.append((x[3], int(x[5].split()[0]), int(int(x[2].split()[0]) / n), x[0]))

df = pd.DataFrame(mem1, columns=["Type", "Freq", "Size", "Price"])
df.to_csv("memory.txt")
