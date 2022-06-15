# Copy II

**Status** » Abandoned
**Language** » C++

*CPii was an failed atempt to reinvent the wheel*

The idea was to code a command wrapper that would perform the copy command while printing on terminal how is the progress going. After showing this early code to a friend he reminded me about the existance of **rsync**.

The command `rsync -a --info=progress2 src dest` does exactly what I wanted to achieve.

<div style="display: flex; justify-content: center;">
    <img src="images/rsync example.png" alt="Gear image as logo" >
</div>
