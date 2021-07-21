class Control extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    this.handleClick = this.handleClick.bind(this)
  }
  
  componentDidMount() {

  }
  
  handleClick() {
    var name = event.target.name
    var data1 = this.props.data1
    
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.visible)
    var userId = parseInt(states.userId)
    var algoName = states.modules.call
    
    if (name == 'add' && data1.cur < data1.max) {
      data1.cur++
      this.props.setStates({noCell: data1})
    }
    
    else if (name == 'subtract' && data1.cur > 1) {
      data1.cur--
      this.props.setStates({noCell: data1})
    }
    
    else if (name == 'position') {
      var command1 = {cmd: 'position', id: userId, algo_name: algoName, ref: 'position'}
      sendWebsocket(JSON.stringify(command1))
      
      if (this.props.data2.position) obj.position = false
      else if (!this.props.data2.position) obj.position = true
    }

    else if (name == 'order') {
      var command1 = {cmd: "order_list", id: userId, algo_name: algoName, max_display: 9999, ref: 'orders'}
      sendWebsocket(JSON.stringify(command1))
      var command2 = {cmd: 'getprofit', id: userId, algo_name: algoName, ref: 'getprofit'}
      sendWebsocket(JSON.stringify(command2))
      
      if (this.props.data2.order) obj.order = false
      else if (!this.props.data2.order) obj.order = true
    }
    
    else if (name == 'command') {
      
      if (this.props.data2.command) obj.command = false
      else if (!this.props.data2.command) obj.command = true
    }
    
    else if (name == 'recommender') {
      if (this.props.data2.recommender == false) obj.recommender = true
      else if (this.props.data2.recommender == true) obj.recommender = false
    }
    
    this.props.setStates({visible: obj})
  }
  
  render() {
    var cssBtnOrder = 'btn-secondary'
    if (this.props.data2.order) cssBtnOrder = 'btn-primary'
    
    var cssBtnPos = 'btn-secondary'
    if (this.props.data2.position) cssBtnPos = 'btn-primary'
    
    var cssBtnCommand = 'btn-secondary'
    if (this.props.data2.command) cssBtnCommand = 'btn-primary'
    
    var cssBtnRecommender = 'btn-secondary'
    if (this.props.data2.recommender) cssBtnRecommender = 'btn-primary'
    
    return(
      <React.Fragment>

<div className="col-12 col-sm-6 col-md-3 mb-2 mb-sm-0">
  <button type="button" name="subtract" className="btn btn-sm btn-secondary" onClick={this.handleClick}>-</button>
  <span className="ml-1 mr-1">{this.props.data1.cur}</span>
  <button type="button" name="add" className="btn btn-sm btn-secondary" onClick={this.handleClick}>+</button>
  <button type="button" name="position" className={classNames("btn btn-sm ml-2 ml-sm-2", cssBtnPos)} onClick={this.handleClick}>仓位</button>
  <button type="button" name="order" className={classNames("btn btn-sm ml-2 ml-sm-2", cssBtnOrder)} onClick={this.handleClick}>当日成交</button>
  {false && <button type="button" name="command" className={classNames("btn btn-sm ml-2 ml-sm-2", cssBtnCommand)} onClick={this.handleClick}>指令</button> }
  <button type="button" name="recommender" className={classNames("btn btn-sm ml-2 ml-sm-2", cssBtnRecommender)} onClick={this.handleClick}>产品推荐</button>
</div>

      </React.Fragment>
    )
  }
}