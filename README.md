# lambda-type-verify

型付きラムダ計算の型検査と型推論をするプログラムです。

## ビルドと実行

```
$ make
$ ./main
```

### 型検査を実行

```
$ ./main -v
```

### 型推論を実行

```
$ ./main -i
```

## 文法

expを任意の文として、

### ラムダ式

```
lambda x.[exp]
```

xは変数名で好きに決めてよい。名前つけのルールはC言語に従う。xに型注釈を入れる場合は

```
lambda x:int.[exp]
```

のようにする（intは好きな型の名前で置き換え可能）

### 関数適用

fを関数として、

```
f([exp])
```

のようにかく。

## プログラム例

### 型推論

```
lambda x.x
lambda x.lambda y.x
lambda x.lambda y.y
lambda x:int.x
lambda x.lambda y:int.y
lambda x.lambda y.(lambda w.lambda z.w)(x)(y)
lambda x.lambda y.(lambda w:int.lambda z.w)(x)(y)
lambda x.lambda y.(lambda w:int.lambda z:str.w)(x)(y)
```

### 型検査

```
f:int->int = lambda x:int.x
f:int->str = lambda x:int.x
f:int->int->int = lambda x:int. lambda y:int.y
f:(int->int)->int->int = lambda f:int->int. lambda x:int. f(x)
f:int->int = lambda x:int.(lambda z:int.z)(x)
f:int->str->int = lambda x:int.lambda y:str.(lambda w:int. lambda z:str.w)(x)(y)
f:int->str->int = lambda x:int.lambda y:str.(lambda w:int. lambda z:str.z)(x)(y)
```
