# Step 1: Modifying HTML #

In order for Weblinguist to be able to find strings inside HTML pages, you need to surround them by the **i18n** tag, like so:

`<i18n>A string to translate.</i18n>`

You can also use other mark-up inside an i18n tag, but keep in mind that too much HTML might make the text less readable for the translator.

`<i18n>Another string to <em>translate</em>.</i18n>`

If you have any external links pointing to pages which will not be translated, you need to add the **external** attribute into the corresponding **a** tags.

`<a href="index.html">Home page</a>`

`<a external href="http://code.google.com"><i18n>Google Code</i18n></a>`

If you need to translate links or if they appear in the middle of a sentence within the **i18n** tag, use **external=""** instead of just **external**. If you find this confusing, always use **external=""** to prevent errors.

`<i18n>Have a look at the <a external="" href="http://code.google.com">Google Code</a> site.</i18n>`

This is required because Weblinguist will automatically add "`?lang=SomeLanguage`" or "`&lang=SomeLanguage`" to the **href** attribute of each internal (i.e. not containing the attribute **external**) link, so that the appropriate language version is loaded when the user clicks on a link.

Say we have a page called index.html, which has several translations (Slovak and French, for instance). We refer to those versions using the **lang** parameter:

`http://some.server.org/index.html?lang=Slovak`

`http://some.server.org/index.html?lang=French`

There are two more minor modifications we need to make to each web page. First, we need to add this line:

`<script type="text/javascript" src="page_name.i18n.js"></script>`

into the **head** section of the HTML. Replace page\_name with the file name of the page (in case of index.html, it is index).

Secondly, we need to add `onLoad="onLoad();"` to the **body** element of the HTML, like this:

`<body onLoad="onLoad();">`

In some cases, you might want to add a link to a certain language version of a page. The best way to do so is as follows:

`<a external href="index.html?lang=Slovak">This page in Slovak</a>`

However, if and **only if** you have one translation alone, you can also use the `translate(lang);` function:

`<a href="javascript:translate('Slovak')">Translate to Slovak</a>`

That's all, we are now ready to proceed to the next step. This is what we have done so far:

File `index.html`:
```
<html>
<head>
    <script type="text/javascript" src="index.i18n.js"></script>
</head>
<body onLoad="onLoad();">
    <i18n>A string to translate.</i18n><br>
    <i18n>Another string to <em>translate</em>.</i18n><br>
    <a external href="index.html">This page in English</a><br>
    <a external href="index.html?lang=Slovak">This page in Slovak</a><br>
    <a external href="index.html?lang=French">This page in French</a><br>
    <a href="another_page.html"><i18n>Another page</i18n></a><br>
    <a external href="http://code.google.com"><i18n>Google Code</i18n></a><br>
    <i18n>Have a look at the <a external="" href="http://code.google.com">Google Code</a> site.</i18n>
</body>
</html>
```

# Step 2: Using Weblinguist #

Now it's time to run Weblinguist. When you do so, create a new project by clicking the "New" button and save it inside the web's root directory.

To add a language, click the "Add language" button. In our case, we add Slovak and French. They will appear in the combo box next to the "Add language" button.

Now add index.html into the project by clicking the "Add" button in the lower left corner. Double click the newly added index.html in the list of files and translate each string one by one. Double click the first of them, enter its translation and use the "Done and next" button to confirm the translation. When finished, switch to a different language using the above mentioned combo box and repeat the process of translation.

If you make a change to an HTML file, click "Update all". Weblinguist will search all files in the project for new strings.

When all the strings are translated, click the "Release" button. That's all there is to it! Everything should work now.