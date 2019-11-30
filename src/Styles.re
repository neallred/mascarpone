let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;

let mascarpone = "#f0eee9";
let raspberry = "#b3111e";
let strawberryStem = "#6ca94f"
let banana = "#fcd182";
let blueberry = "#3e84bf";
let blackCeramic = "#242321";
let baseShadow = "#292f35";
let mascarponeWhite = "#eae6e3";

type colorPalette = {
  base: string,
  warn: string,
  info: string,
  success: string,
  fail: string,
  text: string,
  shadow: string,
  textInverse: string,
};

let palette: colorPalette = {
  base: mascarpone,
  warn: banana,
  info: blueberry,
  success: strawberryStem,
  fail: raspberry,
  text: blackCeramic,
  shadow: baseShadow,
  textInverse: mascarponeWhite,
};

type fontSizes = {
  main: string,
};

let fontSizes = {
  main: "16px",
};


let baseShadow = "2px 2px 2px " ++ palette.shadow;

let baseButton = css(
  ~borderStyle="none",
  ~borderRadius="20px",
  ~padding="5px",
  ~fontSize=fontSizes.main,
  ()
);

let baseInput = css(
  ~fontSize=fontSizes.main,
  ~border="1px solid " ++ palette.text,
  ~borderRadius="2px",
  (),
);

let stylesheet = "
  *, * > * {
    color: " ++ palette.text ++ ";
  }

  body {
    background-color: " ++ palette.base ++ ";
    margin: 0;
  }

  .dns-row:hover .dns-row__controls {
    display: flex !important;
  }
";
