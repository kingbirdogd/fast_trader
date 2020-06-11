import '/component/warrant/tradeRow.css'

class TradeRow extends React.Component {
  static propTypes = {
    id: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    state: PropTypes.shape({
      data: PropTypes.object,
      error: PropTypes.object,
      result: PropTypes.string,
    }),
    handleChange: PropTypes.func,
    handleOnMouseOver: PropTypes.func,
    handleStart: PropTypes.func,
  }
  
  render() {
    var form = this.props.fields.map((field, index) => {
      if(field.type=='text') {
        return <TextField 
          id={this.props.id}
          key={field.name}
          name={field.name}
          placeholder={field.value}
          value={ (this.props.state.data.hasOwnProperty(field.name)) ? this.props.state.data[field.name] : '' }
          error={ (this.props.state.error.hasOwnProperty(field.name)) ? this.props.state.error[field.name] : '' }
          handleChange={this.props.handleChange}
          handleOnMouseOver={this.props.handleOnMouseOver}
        />
      }
      else if(field.type=='button') {
        return <Button 
          id={this.props.id}
          key={field.name}
          name={field.name}
          value={field.value}
          result={this.props.state.result}
          onClick={this.props.handleStart}
        />
      }
    })
    return (
      <React.Fragment>
        {form}
        <Message 
          key={this.props.id}
          result={this.props.state.result}
        />
      </React.Fragment>
    )
  }
}

class Header extends React.Component {
  render() {
    var header = this.props.fields.map((field, index) => {
      return <div key={field.name} className="title">{ field.type=='text' ? field.value : '' }</div>
    })
    return (
      <div className="title_row">{header}</div>
    )
  }
}

class ModuleSelector extends React.Component {
  static propTypes = {
    curModule: PropTypes.string,
    module: PropTypes.array,
    handleChangeModule: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.getText = this.getText.bind(this)
  }
  
  getText(lang) {
    var text = {
      en: {
        algo: {title: 'Algo'},
      },
      sc: {
        algo: {title: '选轮策略'},
      },
      tc: {
        algo: {title: '選輪策略'},
      }
    }
    return text[lang]
  }
  
  render() {
    var options = []
    var text = this.getText(this.props.lang)
    this.props.module.forEach(function(module) {
      options.push(
        <option 
          key={module}
          value={module}>
          {module}
        </option>)
    })
    
    return (
      <div className="config_row">
        <div className="title">{text.algo.title}:</div>
        <select 
          name="fields"
          className="fields"
          value={this.props.curModule}
          onChange={this.props.handleChangeModule}>
            {options}
        </select>
      </div>
    )
  }
}

class NumSelector extends React.Component {
  static propTypes = {
    // curNo: PropTypes.number,
    handleChangeNum: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.getText = this.getText.bind(this)
  }
  
  getText(lang) {
    var text = {
      en: {
        algo: {title: 'No. Trade'},
      },
      sc: {
        algo: {title: '选轮数量'},
      },
      tc: {
        algo: {title: '選輪數量'},
      }
    }
    return text[lang]
  }
  
  render() {
    var options = []
    var text = this.getText(this.props.lang)
    for (var i=1; i<=this.props.totalNo; i++)
      options.push(<option key={i} value={i}>{i}</option>)
    return (
      <div className="config_row">
        <div className="title">{text.algo.title}:</div>
        <select 
          name="fields"
          className="fields"
          value={this.props.curNo}
          onChange={this.props.handleChangeNum}>
          {options}
        </select>
      </div>
    )
  }
}

class TextField extends React.Component {
  static propTypes = {
    id: PropTypes.string,
    name: PropTypes.string,
    placeholder: PropTypes.string,
    value: PropTypes.string,
    error: PropTypes.string,
    onChange: PropTypes.func,
    handleOnMouseOver: PropTypes.func
  }
  
  render() {
    var style = (this.props.error ? "error" : "")
    return (
      <div>
        <input
          className={classNames('field',style)}
          data-id={this.props.id}
          name={this.props.name}
          placeholder={this.props.placeholder}
          value={this.props.value}
          onChange={this.props.handleChange}
          onMouseOver={this.props.handleOnMouseOver}
          autoComplete="off"
          type="text"
        />
        <div className={classNames('response',style)}>
          {this.props.error}
        </div>
      </div>
    )
  }
}

class Button extends React.Component {
  static propTypes = {
    id: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    onClick: PropTypes.func,
  }
  
  render() {
    var style = (this.props.result=='filled') ? "btn-danger" : "btn-dark"
    
    return (
      <input
        className={classNames("btn","btn-sm",style)}
        data-id={this.props.id}
        name={this.props.name}
        value={this.props.value}
        onClick={this.props.onClick}
        type="button"
      />
    )
  }
}

class Message extends React.Component {
  render() {
    return (
      <div className="message">
        {this.props.result}
      </div>
    )
  }
}